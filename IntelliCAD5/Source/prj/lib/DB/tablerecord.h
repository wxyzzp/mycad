/* File  : <DevDir>\source\prj\lib\db\tablerecord.h
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 */

#pragma once

////////////////////////////////////////////////////////////////////////
//	db_tablerecord class: definition
////////////////////////////////////////////////////////////////////////

class DB_CLASS db_tablerecord : public db_handitem
{
	friend class db_table;
	friend class db_blocktabrec;
	friend class db_block;
	friend int db_CompEntLinks(db_BinChange** chglst, db_handitem* oldhip, db_handitem* newhip);

public:

	db_tablerecord();
	db_tablerecord(int type, char* pName);
	db_tablerecord(const db_tablerecord& sour);
	~db_tablerecord();	// Implicitly virtual

	int		ret_deleted();
	int		ret_xrefidx();
	int		ret_bylayer();		// These are for checking for specially
	int		ret_byblock();		// ltype recs
	int		ret_continuous();	// (although they'll work for ANY table records).
	bool	get_2(char* pName, int maxlen);
	bool	get_2(char** ppName);
	char*	retp_name();
	bool	get_70(int *pFlags);
	int		ret_flags();

	void	set_deleted(int deleted);
	void	set_xrefidx(int xrefidx);
	void	set_tabrecidx(int idx);
	bool	set_2(char* pName);
	bool	set_70(int flags);

	//int		entgetcommon(resbuf** ppBeg, resbuf** ppEnd);
	int		entgetcommon(resbuf** ppBeg, resbuf** ppEnd, db_drawing* pDrw);
	int		entmod1common(resbuf** ppRb, db_drawing* pDrw);

private:

	int		m_idx;		// The R12-style table record idx
	int		m_xrefidx;
	char*	m_pName;	//  2
	char	m_flags;	// 70
};
