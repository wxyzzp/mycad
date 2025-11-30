/* C:\ICAD\PRJ\LIB\DB\LAYERINDEX.CPP
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
#include "DbLayerIndex.h"

/***************************** db_layer_index *****************************/

db_layer_index::db_layer_index(void) : db_handitem(DB_LAYER_INDEX) {
    deleted=0;
    nrecs=timeval[0]=timeval[1]=datasz=handdatasz=0; data=handdata=NULL;
	m_pResbufChain = NULL;
	m_OdClass  = NULL;

}
db_layer_index::db_layer_index(const db_layer_index &sour) :
    db_handitem(sour) {  /* Copy constructor */

    deleted=sour.deleted;

    nrecs=sour.nrecs;
    timeval[0]=sour.timeval[0];
    timeval[1]=sour.timeval[1];

    datasz=sour.datasz;
    data=NULL;
    if (datasz>0 && sour.data!=NULL) {
        data=new char[datasz];
        memcpy(data,sour.data,datasz);
    }

    handdatasz=sour.handdatasz;
    handdata=NULL;
    if (handdatasz>0 && sour.handdata!=NULL) {
        handdata=new char[handdatasz];
        memcpy(handdata,sour.handdata,handdatasz);
    }

	m_pResbufChain = NULL;
	m_OdClass  = NULL;
}
db_layer_index::~db_layer_index(void) 
{ 
	sds_relrb(m_pResbufChain);
	OdDbLayerIndexPtr pClass((OdDbLayerIndex*)m_OdClass, kOdRxObjAttach);

	delete [] data; 
	delete [] handdata; 
}

// EBATECH(CNBR) -[ 2002/10/10 Bugzilla#78218
int db_layer_index::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    int rc;
	sds_real stamp;
    struct resbuf *sublist[2];
	int idx, num, clen, hidx;
	char *layer;
    sds_name en;
	unsigned char handbase[8];
	db_objhandle handle;
    db_handitem *hip1;

    rc=0; sublist[0]=sublist[1]=NULL; layer=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 40  Time stamp */
	stamp = (sds_real)timeval[0] + (sds_real)timeval[1]/86400000.0;
    if ((sublist[0]=sublist[1]=db_newrb(40,'R', &stamp))==NULL)
        { rc=-1; goto out; }

    /* Repeated Item */
    for( idx = 0, hidx = 0 ; idx < datasz ; idx += clen + 6, hidx += 8 )
    {
		/*   8 Layer name  */
		clen = *(short *)&data[idx+4];
		layer = new char [clen+1];
		memcpy( layer, &data[idx+6], clen );
		layer[clen] = 0;
		if ((sublist[1]->rbnext=db_newrb(8, 'S', layer))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
		delete [] layer;
		layer = NULL;

		/* 360 Hard owner pointer to IDBUFFER */
		en[0] = en[1] = NULL;
		memcpy(handbase, &handdata[hidx], 8 );
		handle = db_objhandle( handbase );
		if(( hip1 = dp->handent(handle)) != NULL )
			{ en[0] = (long)hip1; en[1] = (long)dp; }
		if ((sublist[1]->rbnext=db_newrb(360,'N', en))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		/*  90 Number of entities  */
		num = *(int *)&data[idx];
		if ((sublist[1]->rbnext=db_newrb(90,'I', &num))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}


out:
	if(layer) delete [] layer;
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}
// EBATECH(CNBR) ]-
