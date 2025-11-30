/* File  : <DevDir>\source\prj\lib\db\table.cpp
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 */

#include "db.h"/*DNT*/

////////////////////////////////////////////////////////////////////////
//	db_table class: implementation
////////////////////////////////////////////////////////////////////////

db_table::db_table() :	m_pNextTabRec(NULL)
{
	m_pTabRecLL[0] = m_pTabRecLL[1] = NULL;
}

db_table::~db_table()
{
    db_handitem::delll(m_pTabRecLL[0]);
}

int
db_table::addRecord
(
 db_handitem*	pTabRec
)
{
//	Adds pTabRec to m_pTabRecLL[0] for this table.
//
//	WARNING: As with db_drawing::addhanditem(), calling this function
//	removes pTabRec from the caller's care: it is either added to the
//	database -- or deleted.
//
//	Returns:
//		 0 : OK
//		-1 : Not added, for some reason (dup or "" table rec name, etc).

	int		rc = 0, hipType;
	char	tabRecName[IC_ACADNMLEN];
	bool	bShape = false;

	if(!pTabRec)
		rc = -1;
	else
	{
		//	In general, records within the same table must have
		//	unique, non-"" names.  The following exceptions apply:
		//
		//		The VPORT, VX and STYLE (SHAPE|REF only) tables can have duplicate record names.
		//			(Well, maybe the VX can only have duplicate ""
		//			record names.  Not sure at this point.)
		//
		//		The VX table can have records named "".
		//
		//		SHAPE entries in the STYLE table are always named "", except when they are from an
		//		Xref, in which case their name is SHAPE|REF, and there can be duplicates of those
		//
		//	Check for compliance with these rules before adding pTabRec
		//	to the table:

		hipType = pTabRec->ret_type();
		pTabRec->get_2(tabRecName, sizeof(tabRecName) - 1);

		if(hipType == DB_STYLETABREC)
		{	// Force "" for SHAPE entries.
			int	flags;
			pTabRec->get_70(&flags);
			bShape = !!(flags & DB_SHAPESTYLE);
			if(bShape && *tabRecName && !strsame(tabRecName, "SHAPE|REF"/*DNT*/))
			{
				*tabRecName = 0;
				pTabRec->set_2(tabRecName);
			}
		}

		if(*tabRecName)
		{
			if(findRecord(tabRecName) && hipType != DB_VPORTTABREC && hipType != DB_VXTABREC && (hipType != DB_STYLETABREC || !strsame(tabRecName, "SHAPE|REF"/*DNT*/)))
				rc = -1;
		}
		else
		{
			if(hipType == DB_STYLETABREC)
			{
				if(!bShape)
					rc = -1;
			}
			else
			{
				if(hipType != DB_VXTABREC)
					rc = -1;
			}
		}

		if(rc)
			delete pTabRec;
		else
		{
			if(!m_pTabRecLL[0])
				m_pTabRecLL[0] = pTabRec;
			else 
				m_pTabRecLL[1]->next = pTabRec;

			if(!m_pNextTabRec)				/*D.G.*/// Update 
				m_pNextTabRec = pTabRec;	// m_pNextTabRec.

			m_pTabRecLL[1] = pTabRec;
		}
	}	// pTabRec != NULL

	return rc;
}

db_handitem*
db_table::findRecord
(
 const char*	pName,
 bool			bValidOnly /* = true */
)
{
	db_handitem*	pTabRec = NULL;

	if(pName)
	{
		char	upper1[DB_ACADBUF], upper2[DB_ACADBUF];
		strcpy(upper1, pName);
		_tcsupr(upper1);

		for(pTabRec = m_pTabRecLL[0]; pTabRec; pTabRec = pTabRec->next)
		{
			if(pTabRec->ret_deleted() && bValidOnly)
				continue;
			strcpy(upper2, ((db_tablerecord*)pTabRec)->m_pName);
			_tcsupr(upper2);
			if(!strcmp(upper1, upper2))
				break;
		}
	}

	return pTabRec;
}

int db_table::check(db_drawing* pDrawing)
{
	db_handitem* pTabRec;
	for(pTabRec = m_pTabRecLL[0]; pTabRec; pTabRec = pTabRec->next)
		if(!pTabRec->check(pDrawing)) // currently it can't fail so just assert
			assert(false);

	return 1;
}
