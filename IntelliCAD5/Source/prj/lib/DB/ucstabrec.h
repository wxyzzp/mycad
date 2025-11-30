// File: <DevDir>\source\prj\lib\db\ucstabrec.h
// Copyright (C) 1988-1998 Visio Corporation. All rights reserved.

#pragma once

/*DG - 6.6.2002*/// Added support of groups 79, 71, 146, 346, 13.

class DB_CLASS db_ucstabrec : public db_tablerecord
{
	friend int db_CompEntLinks(db_BinChange** chglst, db_handitem* oldhip, db_handitem* newhip);

public:
	db_ucstabrec();
	db_ucstabrec(char* pname, db_drawing* dp);
	db_ucstabrec(const db_ucstabrec& sour);

	bool get_10(sds_point p)	{ p[0] = m_orig[0]; p[1] = m_orig[1]; p[2] = m_orig[2]; return true; }
	bool get_11(sds_point p)	{ p[0] = m_xdir[0]; p[1] = m_xdir[1]; p[2] = m_xdir[2]; return true; }
	bool get_12(sds_point p)	{ p[0] = m_ydir[0]; p[1] = m_ydir[1]; p[2] = m_ydir[2]; return true; }

	bool set_10(sds_point p)	{ m_orig[0] = p[0]; m_orig[1] = p[1]; m_orig[2] = p[2]; return true; }
	bool set_11(sds_point p)	{ m_xdir[0] = p[0]; m_xdir[1] = p[1]; m_xdir[2] = p[2]; return true; }
	bool set_12(sds_point p)	{ m_ydir[0] = p[0]; m_ydir[1] = p[1]; m_ydir[2] = p[2]; return true; }

	bool get_79(short* pOrthoViewType)		{ *pOrthoViewType = m_orthoViewType; return true; }
	bool get_71(short* pOrthoType)			{ *pOrthoType = m_orthoType; return true; }
	bool get_146(sds_real* pElevation)		{ *pElevation = m_elevation; return true; }
	bool get_346(db_handitem** ppBaseUcs)	{ *ppBaseUcs = m_pBaseUcs; return true; }
	db_handitem** retp_baseucs()			{ return &m_pBaseUcs; }
	bool get_13(sds_point p)				{ p[0] = m_orthoOrig[0]; p[1] = m_orthoOrig[1]; p[2] = m_orthoOrig[2]; return true; }

	bool set_79(short orthoViewType)		{ m_orthoViewType = orthoViewType; return true; }
	bool set_71(short orthoType)			{ m_orthoType = orthoType; return true; }
	bool set_146(sds_real elevation)		{ m_elevation = elevation; return true; }
	bool set_346(db_handitem* pBaseUcs)		{ m_pBaseUcs = pBaseUcs; return true; }
	bool set_13(sds_point p)				{ m_orthoOrig[0] = p[0]; m_orthoOrig[1] = p[1]; m_orthoOrig[2] = p[2]; return true; }

private:
	sds_point		m_orig,				// 10
					m_xdir,				// 11
					m_ydir,				// 12
					m_orthoOrig;		// 13
	short			m_orthoViewType,	// 79
					m_orthoType;		// 71
	sds_real		m_elevation;		// 146
	db_handitem*	m_pBaseUcs;			// 346
};
