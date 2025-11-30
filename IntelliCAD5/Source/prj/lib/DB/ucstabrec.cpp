// File: <DevDir>\source\prj\lib\db\ucstabrec.cpp
// Copyright (C) 1988-1998 Visio Corporation. All rights reserved.

#include "db.h"
#include "ucstabrec.h"


db_ucstabrec::db_ucstabrec() :
	db_tablerecord(DB_UCSTABREC, db_str_quotequote),
	m_orthoViewType(0), m_orthoType(0), m_elevation(0.0), m_pBaseUcs(NULL)
{
	m_orthoOrig[0] = 0.0, m_orthoOrig[1] = 0.0, m_orthoOrig[2] = 0.0;
	db_qgetvar(DB_QUCSORG,  NULL, m_orig, DB_3DPOINT, 0);
	db_qgetvar(DB_QUCSXDIR, NULL, m_xdir, DB_3DPOINT, 0);
	db_qgetvar(DB_QUCSYDIR, NULL, m_ydir, DB_3DPOINT, 0);
}

db_ucstabrec::db_ucstabrec(char* pname, db_drawing* dp) :
	db_tablerecord(DB_UCSTABREC, pname),
	m_orthoViewType(0), m_orthoType(0), m_elevation(0.0), m_pBaseUcs(NULL)
{
	m_orthoOrig[0] = 0.0, m_orthoOrig[1] = 0.0, m_orthoOrig[2] = 0.0;

	char*	bufp = dp ? dp->ret_headerbuf() : NULL;
	bool	inps = dp ? dp->inpspace() : false;

	db_qgetvar(inps ? DB_QP_UCSORG  : DB_QUCSORG , bufp, m_orig, DB_3DPOINT, 0);
	db_qgetvar(inps ? DB_QP_UCSXDIR : DB_QUCSXDIR, bufp, m_xdir, DB_3DPOINT, 0);
	db_qgetvar(inps ? DB_QP_UCSYDIR : DB_QUCSYDIR, bufp, m_ydir, DB_3DPOINT, 0);
}

db_ucstabrec::db_ucstabrec(const db_ucstabrec& sour) :
	db_tablerecord(sour),
	m_orthoViewType(sour.m_orthoViewType), m_orthoType(sour.m_orthoType),
	m_elevation(sour.m_elevation), m_pBaseUcs(sour.m_pBaseUcs)
{
	DB_PTCPY(m_orthoOrig, sour.m_orthoOrig);
	DB_PTCPY(m_orig, sour.m_orig);
	DB_PTCPY(m_xdir, sour.m_xdir);
	DB_PTCPY(m_ydir, sour.m_ydir);
}
