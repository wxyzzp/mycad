/* C:\ICAD\PRJ\LIB\DB\SPATIAL_FILTER.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "objects.h"

/***************************** db_spatial_filter **************************/

db_spatial_filter::db_spatial_filter(void) : db_handitem(DB_SPATIAL_FILTER) {
    deleted=0;
    npts=clipon[0]=clipon[1]=dxf71=0; pt=NULL;
    dxf11[0]=dxf11[1]=dxf11[2]=clip[0]=clip[1]=0.0; extru=NULL;
    memset(inverseblockmat,0,sizeof(inverseblockmat));
    inverseblockmat[0][0]=inverseblockmat[1][1]=inverseblockmat[2][2]=1.0;
    memset(clipboundmat,0,sizeof(clipboundmat));
    clipboundmat[0][0]=clipboundmat[1][1]=clipboundmat[2][2]=1.0;
}
db_spatial_filter::db_spatial_filter(const db_spatial_filter &sour) :
    db_handitem(sour) {  /* Copy constructor */

    deleted=sour.deleted;

    clipon[0]=sour.clipon[0];
    clipon[1]=sour.clipon[1];
    dxf71    =sour.dxf71;
    clip[0]  =sour.clip[0];
    clip[1]  =sour.clip[1];
    DB_PTCPY(dxf11,sour.dxf11);

    extru=NULL;  /* Don't use db_aextrucpy() here; wastes time unitizing. */
    if (sour.extru!=NULL) { extru=new sds_real[3]; DB_PTCPY(extru,sour.extru); }

    npts=sour.npts; pt=NULL;
    if (npts>0 && sour.pt!=NULL) {
        pt=new sds_point[npts]; memcpy(pt,sour.pt,npts*sizeof(pt[0]));
    }

    memcpy(inverseblockmat,sour.inverseblockmat,sizeof(inverseblockmat));
    memcpy(clipboundmat,sour.clipboundmat,sizeof(clipboundmat));
}
db_spatial_filter::~db_spatial_filter(void) {
    delete [] pt; delete [] extru;
}

// EBATECH(CNBR) -[ 2002/10/10
int db_spatial_filter::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    int rc;
    struct resbuf *sublist[2];
	int fint1, fint2;
    short fsh1;
	sds_point pt;
	sds_point *ppt = NULL;

    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 70  Number of coordinates */
    fsh1 = fint2 = ret_npts();
    if ((sublist[0]=sublist[1]=db_newrb(70,'H',&fsh1))==NULL)
        { rc=-1; goto out; }

    /* 10  Clippling edge coordinates(2 or more) */
	if(( ppt = new sds_point[fint2]) == NULL )
		{ rc=-1; goto out; }
	get_ptarray(ppt, fint2);
	for( fint1 = 0 ; fint1 < fint2 ; fint1++ ){
		if ((sublist[1]->rbnext=db_newrb(10,'P',ppt[fint1]))==NULL){
			delete [] ppt;
			ppt = NULL;
			rc=-1;
			goto out;
		}
		sublist[1]=sublist[1]->rbnext;
	}
	delete [] ppt;
	ppt = NULL;

    /* 210 Extrusion vector */
    get_extru(pt);
    if ((sublist[1]->rbnext=db_newrb(210,'P',pt))==NULL)
        { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 11 origin of local coordinate system */
    if ((sublist[1]->rbnext=db_newrb(11,'P',dxf11))==NULL)
        { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 71 cliiping frame display flag */
    fsh1 = (short)dxf71;
    if ((sublist[1]->rbnext=db_newrb(71,'H',&fsh1))==NULL)
        { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 72 front clipping */
    fsh1 = (short)clipon[0];
    if ((sublist[1]->rbnext=db_newrb(72,'H',&fsh1))==NULL)
        { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 40 front cliiping distance(optional) */
	if( clipon[0] ) {
		if ((sublist[1]->rbnext=db_newrb(40,'R',&clip[0]))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

    /* 73 back clipping */
    fsh1 = (short)clipon[1];
    if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL)
        { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 41 back cliiping distance(optional) */
	if( clipon[1] ) {
		if ((sublist[1]->rbnext=db_newrb(41,'R',&clip[0]))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

    /* 40 * 12 Transformation Matrix 1 */
    for( fint1 = 0 ; fint1 < 4 ; ++fint1 ){
    	for( fint2 = 0 ; fint2 < 3 ; ++fint2 ){
			if ((sublist[1]->rbnext=db_newrb(40,'R',&inverseblockmat[fint1][fint2]))==NULL)
				{ rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;
		}
	}

    /* 40 * 12 Transformation Matrix 2 */
    for( fint1 = 0 ; fint1 < 4 ; ++fint1 ){
    	for( fint2 = 0 ; fint2 < 3 ; ++fint2 ){
			if ((sublist[1]->rbnext=db_newrb(40,'R',&clipboundmat[fint1][fint2]))==NULL)
				{ rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;
		}
	}

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

int db_spatial_filter::entmod(struct resbuf *modll, db_drawing *dp)
{
/*
**  Entity/table-record/object specific.  Uses the setters for range-correction.
**
**  Returns: See db_handitem::entmod().
*/
    int rc;
    struct resbuf *curlink;
    db_handitem *thiship;
    sds_point* ppt = NULL;
    int	i, n, i2;
    rc=0; thiship=(db_handitem *)this;

    db_lasterror=0;

    if (modll==NULL || dp==NULL) { rc=-1; goto out; }

    del_xdict();

    curlink=modll;

    /*
    **  If modll has been built properly, the first few groups
    **  are things we don't allow to be modified (ename, type, handle),
    **  so let's walk over these right now:
    */
    while (curlink!=NULL &&
          (curlink->restype==-1 ||
           curlink->restype== 0 ||
           curlink->restype== 5)) curlink=curlink->rbnext;

    /* Walk; stop at the end or the EED sentinel (-3): */
    i = i2 = 0;
    for (; !rc && curlink!=NULL && curlink->restype!=-3; curlink=curlink->rbnext) {
        /* Call the correct setter: */
        switch (curlink->restype) {
		case 70:
			n = curlink->resval.rint;
			if( n < 2 ){
				rc = -1; goto out;
			}
			ppt = new sds_point[n];
			if( ppt == NULL ) {
				rc = -1; goto out;
			}
			break;
		case 10:
			if( ppt && i < n ){
				ppt[i][0] = curlink->resval.rpoint[0];
				ppt[i][1] = curlink->resval.rpoint[1];
				ppt[i][2] = curlink->resval.rpoint[2];
				++i;
			}
			if( i == n ){
				set_ptarray( ppt, n );
				delete [] ppt;
				ppt = NULL;
			}
		case 210:
			set_extru( curlink->resval.rpoint );
			break;
		case 11:
			set_11( curlink->resval.rpoint );
			break;
		case 71:
			set_71( curlink->resval.rint );
			break;
		case 72: // and 40
			set_frontclipon( curlink->resval.rint );
			if( curlink->rbnext->restype == 40 ){
				curlink=curlink->rbnext;
				set_frontclip( curlink->resval.rreal );
			}
			break;
		case 73: // and 41
			set_backclipon( curlink->resval.rint );
			if( curlink->rbnext->restype == 41 ){
				curlink=curlink->rbnext;
				set_backclip( curlink->resval.rreal );
			}
			break;
		case 40:
			if( i2 == 0 ){
				ppt = new sds_point[4];
				if( ppt == NULL ) {
					rc = -1; goto out;
				}
			}
			if( i2 < 12 ){
				ppt[ i2 / 3 ][ i2 % 3 ] = curlink->resval.rreal;
			}
			else if( i2 < 24 ){
				ppt[(i2-12)/ 3 ][(i2-12)% 3 ] = curlink->resval.rreal;
			}
			++i2;
			if( i2 == 12 && ppt ){
				set_inverseblockmat( ppt );
			}
			if( i2 == 24 && ppt ){
				set_clipboundmat( ppt );
				delete [] ppt;
				ppt = NULL;
			}
			break;
        default: /* In the case of an OBJECT, this virtual function */
                 /* calls entmodhi(). */
            rc=thiship->entmod1common(&curlink,dp);
            break;
        }
    }
    if( ppt )
    	{ delete [] ppt; rc=-1; goto out; }
    if( i2 != 0 && i2 != 12 && i2 != 24 )
    	{ rc=-1; goto out; }

    if (db_lasterror) { rc=-6; goto out; }

    if (!rc && curlink!=NULL) rc=set_eed(curlink,dp);

out:

    return rc;
}
// EBATECH(CNBR) ]-
