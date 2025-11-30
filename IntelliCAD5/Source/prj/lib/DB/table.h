/* File  : <DevDir>\source\prj\lib\db\table.h
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 */

#pragma once

////////////////////////////////////////////////////////////////////////
//	db_table class: definition
////////////////////////////////////////////////////////////////////////

class DB_CLASS db_table
{
	friend class db_drawing;

public:

	db_table();
	~db_table();

/*-------------------------------------------------------------------------*//**
Checks table for validity and fix errors if possible

@author Denis Petrov
@param pDrawing => pointer to the drawing
@return 1 if table is valid or successfully validated, 0 else or for error
*//*------------------------------------------------------------------------*/
	int check(db_drawing* pDrawing);
private:

	int				addRecord(db_handitem* pTabRec);
	db_handitem*	findRecord(const char* pName, bool bValidOnly = true);


	db_handitem*	m_pTabRecLL[2];	// Linked list of records (0 = head; 1 = tail)
	db_handitem*	m_pNextTabRec;	// A ptr into m_pTabRecLL[0] (tblnext).
									// DON'T delete.
};
