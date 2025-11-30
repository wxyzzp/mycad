/* C:\ICAD\PRJ\LIB\DB\VBAPROJECT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "objects.h"

#include "OdaCommon.h"
#include "..\..\..\Licensed\DWGdirect\Include\ResBuf.h" // clash with icadlib\resbuf.h
#include "DbVbaProject.h"

/***************************** db_vbaproject *****************************/

db_vbaproject::db_vbaproject(void) : db_handitem(DB_VBAPROJECT) {
	deleted=0;	// Ebatech(cnbr) 2002/10/10
	datasize=0;
	data=NULL;
	}

db_vbaproject::db_vbaproject(const db_vbaproject &sour) :
	db_handitem(sour) {  /* Copy constructor */
	deleted=sour.deleted;	// Ebatech(cnbr) 2002/10/10
	datasize=sour.datasize;
	data= new char [datasize];
	memcpy(data,sour.data,datasize);
}
db_vbaproject::~db_vbaproject(void)
{
	delete [] data;
}

// EBATECH(CNBR) -[ 2002/10/10 Bugzilla#78218
int db_vbaproject::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
/*
**	Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
	int rc;
	int fint1;
	char bbuf[128];
	struct resbuf *sublist[2];

	rc=0; sublist[0]=sublist[1]=NULL;

	if (begpp==NULL || endpp==NULL) goto out;

	/* 90 Data size */
	if ((sublist[0]=sublist[1]=db_newrb(90,'I', &datasize))==NULL)
		{ rc=-1; goto out; }

	/* 310 Object data(s) */
	/* Note : VBA_PROJECT is too large for printing. */
	for( fint1 = 0 ; fint1 < datasize ; fint1 += 127 )
	{
		bbuf[0] = ( fint1 + 127 > datasize ) ? datasize - fint1 : 127;
		memcpy( &bbuf[1], data+fint1, bbuf[0] );
		if ((sublist[1]->rbnext=db_newrb(310,'B',bbuf))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

out:
	if (begpp!=NULL && endpp!=NULL) {
		if (rc && sublist[0]!=NULL)
			{ sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
		*begpp=sublist[0]; *endpp=sublist[1];
	}
	return rc;
}
// EBATECH(CNBR) ]-
