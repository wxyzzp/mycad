/* C:\ICAD\PRJ\LIB\DB\VERTEX.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_vertex *****************************/

/*F*/
db_vertex::db_vertex(void) : db_entity(DB_VERTEX) {

    pt[0]=pt[1]=pt[2]=wid[0]=wid[1]=bulge=tandir=0.0; flags=0; vidx=NULL;
	m_pParentPolyline = NULL;
}
db_vertex::db_vertex(db_drawing *dp) : db_entity(DB_VERTEX,dp) {

    pt[0]=pt[1]=pt[2]=wid[0]=wid[1]=bulge=tandir=0.0;
	flags=0; vidx=NULL;
	m_pParentPolyline = NULL;
	if ( dp != NULL )
		{
		m_pParentPolyline = (db_polyline *)dp->ret_complexmain();
		ASSERT( m_pParentPolyline != NULL );
		ASSERT( m_pParentPolyline->ret_type() == DB_POLYLINE );
		}
}

db_vertex::db_vertex(const db_vertex &sour) :
    db_entity(sour) {  /* Copy constructor */

    DB_PTCPY(pt,sour.pt);

    vidx=NULL;
    if (sour.vidx!=NULL)
        { vidx=new int[4]; memcpy(vidx,sour.vidx,4*sizeof(int)); }

    wid[0]=sour.wid[0]; 
	wid[1]=sour.wid[1]; 
	bulge=sour.bulge;
    tandir=sour.tandir;  
	flags=sour.flags;
	// don't copy this
	//
	m_pParentPolyline = NULL;
}
db_vertex::~db_vertex(void) {

    delete [] vidx;
}

/*F*/
int db_vertex::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    short fsh1;
    int rc,fi1;
	struct resbuf *sublist[2];

	
	rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

	if(db_handitem* pMain = ret_main())
    {
		sds_name name;
		name[0] = (long)pMain;
		name[1] = (long)dp;
		if ((sublist[0]=sublist[1]=db_newrb(330, 'N', name))==NULL) { rc=-1; goto out; }
		/* 10 (pt) */
		if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}
	else
	{
		/* 10 (pt) */
		if ((sublist[0]=sublist[1]=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
	}

    /* 40 (wid[0]) */
    if ((sublist[1]->rbnext=db_newrb(40,'R',wid))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 41 (wid[1]) */
    if ((sublist[1]->rbnext=db_newrb(41,'R',wid+1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 42 (bulge) */
    if ((sublist[1]->rbnext=db_newrb(42,'R',&bulge))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 70 (flags) */
    fsh1=(unsigned char)flags;
    if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 50 (tandir) */
    if ((sublist[1]->rbnext=db_newrb(50,'R',&tandir))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    for (fi1=0; fi1<4; fi1++) {
        fsh1=(short)((vidx!=NULL) ? vidx[fi1] : 0);
        if ((sublist[1]->rbnext=db_newrb(71+fi1,'H',&fsh1))==NULL) { rc=-1; goto out; }
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





