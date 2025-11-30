/* File  : <DevDir>\source\prj\lib\db\tablerecord.cpp
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 */ 

#include "db.h"/*DNT*/

////////////////////////////////////////////////////////////////////////
//	db_tablerecord class: implementation
////////////////////////////////////////////////////////////////////////

db_tablerecord::db_tablerecord() : m_pName(NULL), m_flags(0), m_idx(0), m_xrefidx(-1)
{
}

db_tablerecord::db_tablerecord(int type, char* pName) : db_handitem(type),
	m_pName(NULL), m_flags(0), m_idx(0), m_xrefidx(-1)
{
	db_astrncpy(&m_pName, pName ? pName : db_str_quotequote, -1);
}

db_tablerecord::db_tablerecord(const db_tablerecord& sour) : db_handitem(sour),
	m_pName(NULL), m_flags(sour.m_flags), m_idx(sour.m_idx), m_xrefidx(sour.m_xrefidx)
{
	if(sour.m_pName)
		db_astrncpy(&m_pName, sour.m_pName, -1);
}

db_tablerecord::~db_tablerecord()
{
    delete [] m_pName;
}

inline int
db_tablerecord::ret_deleted()
{
	return (m_flags & DB_DELETED) != 0;
}

inline int
db_tablerecord::ret_xrefidx()
{
	return m_xrefidx;
}

inline void
db_tablerecord::set_xrefidx(int xrefidx)
{
	m_xrefidx = xrefidx;
}

inline int
db_tablerecord::ret_bylayer()
{
	return !db_compareNames(db_str_bylayer, m_pName);
}

inline int
db_tablerecord::ret_byblock()
{
	return !db_compareNames(db_str_byblock, m_pName);
}

inline int
db_tablerecord::ret_continuous()
{
	return !db_compareNames(db_str_continuous, m_pName);
}

inline bool
db_tablerecord::get_2(char* pName, int maxlen)
{
	if(!m_pName)
		*pName = 0;
	else
	{
		strncpy(pName, m_pName, maxlen);
		pName[maxlen] = 0;
	}
	return true;
}

inline bool
db_tablerecord::get_2(char** ppName)
{
	*ppName = m_pName;
	return true;
}

inline char*
db_tablerecord::retp_name()
{
	return m_pName;
}

inline bool
db_tablerecord::get_70(int* pFlags)
{
	*pFlags = m_flags;
	return true;
}

inline int
db_tablerecord::ret_flags()
{
	return m_flags;
}

inline void
db_tablerecord::set_deleted(int deleted)
{
	if(deleted)
		m_flags |= DB_DELETED;
	else
		m_flags &= ~DB_DELETED;
}

inline void
db_tablerecord::set_tabrecidx(int idx)
{
	m_idx = idx;
}

inline bool
db_tablerecord::set_2(char* pName)
{
	db_astrncpy(&m_pName, pName ? pName : db_str_quotequote, IC_ACADNMLEN - 1);
	return true;
}

inline bool
db_tablerecord::set_70(int flags)
{
	m_flags = flags;
	return true;
}

//int db_tablerecord::entgetcommon(resbuf** ppBeg, resbuf** ppEnd)
int db_tablerecord::entgetcommon(resbuf** ppBeg, resbuf** ppEnd, db_drawing * dp)
{
//	Builds the part of the entget llist for the group codes
//	common to all table records.
//
//	Returns:
//		 0 : OK
//		-1 : No memory

    short	tmpFlags;
    int		rc = 0;
    resbuf*	sublist[2] = {NULL, NULL};

    if(!ppBeg || !ppEnd)
		goto out;

    // 2 (name)
    if(!(sublist[0] = sublist[1] = db_newrb(2, 'S', m_pName ? m_pName : db_str_quotequote)))
	{
		rc = -1;
		goto out;
	}

    // 70 (flags)
    tmpFlags = (short)m_flags;
    if(!(sublist[1]->rbnext = db_newrb(70, 'H', &tmpFlags)))
	{
		rc = -1;
		goto out;
	}
    sublist[1] = sublist[1]->rbnext;

out:
	if(ppBeg && ppEnd)
	{
		if(rc && sublist[0])
		{
			sds_relrb(sublist[0]);
			sublist[0] = sublist[1] = NULL;
		}

		*ppBeg = sublist[0];
		*ppEnd = sublist[1];
	}

	return rc;
}

int
db_tablerecord::entmod1common(resbuf** ppRb, db_drawing* pDrw)
{
//	Returns: See db_handitem::entmod().

	int				rc = 0;
	db_handitem*	thiship = (db_handitem*)this;

	if(!ppRb || !*ppRb || !pDrw)
	{
		rc = -1;
		goto out;
	}

	// Call the correct setter:
	switch((*ppRb)->restype)
	{
		case  2 :
			thiship->set_2((*ppRb)->resval.rstring);
			break;

		case 70 :
			thiship->set_70((*ppRb)->resval.rint);
			break;

		default :
			entmodhi(ppRb, pDrw);	// Catches persistent reactors and extension dictionary.
	}

out:
	return rc;
}
