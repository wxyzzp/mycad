/* C:\ICAD\PRJ\LIB\DB\SPATIAL_INDEX.CPP
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
#include "DbSpatialIndex.h"

/***************************** db_spatial_index *****************************/
db_spatial_index::db_spatial_index(void) : db_handitem(DB_SPATIAL_INDEX) {
    deleted=0;
    databits=timeval[0]=timeval[1]=datasz=0; data=NULL;
    // EBATECH(CNBR) -[ 2002/7/4 Bugzilla78218
    dxf40[0]=dxf40[1]=dxf40[2]=dxf40[3]=dxf40[4]=dxf40[5]=0.0;
    parentinsert=NULL;
    data310sz=0; data310=NULL;
	// EBATECH(CNBR) ]-
	m_pResbufChain = NULL;
	m_OdClass  = NULL;
}
db_spatial_index::db_spatial_index(const db_spatial_index &sour) :
    db_handitem(sour) {  /* Copy constructor */

    deleted=sour.deleted;

    databits  =sour.databits;
    timeval[0]=sour.timeval[0];
    timeval[1]=sour.timeval[1];

    datasz=sour.datasz;
    data=NULL;
    if (datasz>0 && sour.data!=NULL) {
        data=new char[datasz];
        memcpy(data,sour.data,datasz);
    }
    // EBATECH(CNBR) -[ 2002/7/4 Bugzilla78218
    dxf40[0]=sour.dxf40[0];
    dxf40[1]=sour.dxf40[1];
    dxf40[2]=sour.dxf40[2];
    dxf40[3]=sour.dxf40[3];
    dxf40[4]=sour.dxf40[4];
    dxf40[5]=sour.dxf40[5];
    parentinsert=sour.parentinsert;
    data310sz=sour.data310sz;
    data310=NULL;
    if (data310sz>0 && sour.data310!=NULL) {
        data310=new char[data310sz];
        memcpy(data310,sour.data310,data310sz);
    }
	// EBATECH(CNBR) ]-
	m_pResbufChain = NULL;
	m_OdClass  = NULL;
}
db_spatial_index::~db_spatial_index(void) 
{
	sds_relrb(m_pResbufChain);

	OdDbSpatialIndexPtr pClass((OdDbSpatialIndex*)m_OdClass, kOdRxObjAttach);

	delete [] data;
	delete [] data310;	// EBATECH(CNBR) 2002/7/4 Bugzilla78218
}

// EBATECH(CNBR) -[ 2002/7/4 Bugzilla78218
int db_spatial_index::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    int rc;
	sds_real fr1;
    struct resbuf *sublist[2];
	int fint1;
	char bbuf[128];

    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 40  Time stamp */
	fr1 = (sds_real)timeval[0] + (sds_real)timeval[1]/86400000.0;
    if ((sublist[0]=sublist[1]=db_newrb(40,'R', &fr1))==NULL)
        { rc=-1; goto out; }

    /* 40*6 Unknown double value */
    for( fint1 = 0 ; fint1 < 6 ; fint1++ ){
		if ((sublist[1]->rbnext=db_newrb(40,'R',&dxf40[fint1]))==NULL)
			{ rc=-1; goto out; }
	    sublist[1]=sublist[1]->rbnext;
    }

    /* 90 Unknown Int value (always zero) */
    fint1 = 0;
    if ((sublist[1]->rbnext=db_newrb(90,'I',&fint1))==NULL)
        { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 90 Object data byte size */
    if ((sublist[1]->rbnext=db_newrb(90,'I',&data310sz))==NULL)
        { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 310 Object data(s) */
    for( fint1 = 0 ; fint1 < data310sz ; fint1 += 127 )
    {
		bbuf[0] = ( fint1 + 127 > data310sz ) ? data310sz - fint1 : 127;
		memcpy( &bbuf[1], data310+fint1, bbuf[0] );
		if ((sublist[1]->rbnext=db_newrb(310,'B',bbuf))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

    /* 1  End marker of object id section */
    if ((sublist[1]->rbnext=db_newrb(1,'S',"END ACDBSPATIAL BINARY DATA"/*DNT*/))==NULL)
        { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}
// EBATECH(CNBR) ]-
