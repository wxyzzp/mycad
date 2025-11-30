/* C:\ICAD\PRJ\LIB\DB\LTYPETABREC.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 


/************************** db_ltypetabrec **************************/

#include "db.h"
#include "ltypetabrec.h"

/*F*/
db_ltypetabrec::db_ltypetabrec(void) :
    db_tablerecord(DB_LTYPETABREC,db_str_quotequote) {

    dash=NULL; desc=NULL; db_astrncpy(&desc,db_str_continuous,-1);
    aligncd='A'; ndashes=0;
	xrefblkhip=NULL;
	}
db_ltypetabrec::db_ltypetabrec(char *pname, db_drawing *dp) :
    db_tablerecord(DB_LTYPETABREC,pname) {

    dash=NULL; desc=NULL; db_astrncpy(&desc,db_str_continuous,-1);
    aligncd='A'; ndashes=0;
	xrefblkhip=NULL;
}
db_ltypetabrec::db_ltypetabrec(const db_ltypetabrec &sour) :
    db_tablerecord(sour) {  /* Copy constructor */
    int fi1;

    aligncd=sour.aligncd;
	ndashes=sour.ndashes;
    desc=NULL; if (sour.desc!=NULL) db_astrncpy(&desc,sour.desc,-1);
    dash=NULL;
	xrefblkhip=sour.xrefblkhip;
    if (ndashes>0 && sour.dash!=NULL) {
        dash=new db_ltypedashlink[ndashes];
        memcpy(dash,sour.dash,ndashes*sizeof(dash[0]));
        for (fi1=0; fi1<ndashes; fi1++) {
            dash[fi1].text=NULL;
            if (sour.dash[fi1].text!=NULL)
                db_astrncpy(&dash[fi1].text,sour.dash[fi1].text,-1);
        }
    }
}
db_ltypetabrec::~db_ltypetabrec(void) { delete [] desc; delete [] dash; }

/*F*/
void db_ltypetabrec::get_dashdata(
    int           didx,
    char        **textpp,
    sds_real     *xoffsetp,
    sds_real     *yoffsetp,
    sds_real     *scalep,
    sds_real     *lenp,
    sds_real     *rotp,
    int          *typep,
    int          *shapecdp,
    db_objhandle *stylehandle) {


	if (dash!=NULL && didx>-1 && didx<ndashes) {
        if (textpp   !=NULL) *textpp   =dash[didx].text;
        if (xoffsetp !=NULL) *xoffsetp =dash[didx].offset[0];
        if (yoffsetp !=NULL) *yoffsetp =dash[didx].offset[1];
        if (scalep   !=NULL) *scalep   =dash[didx].scale;
        if (lenp     !=NULL) *lenp     =dash[didx].len;
        if (rotp     !=NULL) *rotp     =dash[didx].rot;
        if (typep    !=NULL) *typep    =dash[didx].type;
        if (shapecdp !=NULL) *shapecdp =dash[didx].shapecd;
        if (dash[didx].stylehip==NULL) stylehandle->Nullify();
		else {
			*stylehandle=dash[didx].stylehip->RetHandle();
		}
    }
}

/*F*/
void db_ltypetabrec::set_dashdata(
    int          didx,
    char        *text,
    sds_real     xoffset,
    sds_real     yoffset,
    sds_real     scale,
    sds_real     len,
    sds_real     rot,
    int          type,
    int          shapecd,
    db_objhandle stylehandle,
    db_drawing  *dp) {

    if (dash!=NULL && didx>-1 && didx<ndashes) {
        delete [] dash[didx].text; dash[didx].text=NULL;
            db_astrncpy(&dash[didx].text,text,-1);
        dash[didx].offset[0]=xoffset;
        dash[didx].offset[1]=yoffset;
        dash[didx].scale    =(icadRealEqual(scale,0.0)) ? 1.0 : scale;
        dash[didx].len      =len;
        dash[didx].rot      =rot; ic_normang(&dash[didx].rot,NULL);
        dash[didx].type     =type;
        dash[didx].shapecd  =shapecd;
        if (dp==NULL || stylehandle==NULL) dash[didx].stylehip=NULL;
		else {
			dash[didx].stylehip=dp->handent(stylehandle);
		}
	}
}

/*F*/
int db_ltypetabrec::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    short fsh1,didx;
    int rc;
    sds_real ar1;
    sds_name en;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 3 (desc) */
    if ((sublist[0]=sublist[1]=db_newrb(3,'S',
        (desc!=NULL) ? desc : db_str_quotequote))==NULL) { rc=-1; goto out; }

    /* 72 (aligncd) */
    fsh1=(short)aligncd;
    if ((sublist[1]->rbnext=db_newrb(72,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 73 (ndashes) */
    fsh1=(short)ndashes;
    if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 40 (total pattern length) */
    ar1=0.0; get_40(&ar1);
    if ((sublist[1]->rbnext=db_newrb(40,'R',&ar1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    for (didx=0; didx<ndashes && didx<DB_MAXLTDASHES; didx++) {
        /* 49 (dash length) */
        if ((sublist[1]->rbnext=db_newrb(49,'R',&dash[didx].len))==NULL) { rc=-1; goto out; }
        sublist[1]=sublist[1]->rbnext;

        /* 74 (type) */
        fsh1=(short)dash[didx].type;
        if ((sublist[1]->rbnext=db_newrb(74,'H',&fsh1))==NULL) { rc=-1; goto out; }
        sublist[1]=sublist[1]->rbnext;

		if(fsh1==0) continue;	// EBATECH(CNBR) 2002/4/19 Suppress following items

        /* 75 (shapecd) */
        fsh1=(short)dash[didx].shapecd;
        if ((sublist[1]->rbnext=db_newrb(75,'H',&fsh1))==NULL) { rc=-1; goto out; }
        sublist[1]=sublist[1]->rbnext;

        /* 340 (stylehip) */
        en[0]=en[1]=0;
        if (dash[didx].stylehip!=NULL)
            { en[0]=(long)dash[didx].stylehip; en[1]=(long)dp; }
        if ((sublist[1]->rbnext=db_newrb(340,'N',en))==NULL) { rc=-1; goto out; }
        sublist[1]=sublist[1]->rbnext;

        /* 46 (scale) */
        if ((sublist[1]->rbnext=db_newrb(46,'R',&dash[didx].scale))==NULL) { rc=-1; goto out; }
        sublist[1]=sublist[1]->rbnext;

        /* 50 (rot) */
        if ((sublist[1]->rbnext=db_newrb(50,'R',&dash[didx].rot))==NULL) { rc=-1; goto out; }
        sublist[1]=sublist[1]->rbnext;

        /* 44 (offset[0]) */
        if ((sublist[1]->rbnext=db_newrb(44,'R',dash[didx].offset))==NULL) { rc=-1; goto out; }
        sublist[1]=sublist[1]->rbnext;

        /* 45 (offset[1]) */
        if ((sublist[1]->rbnext=db_newrb(45,'R',dash[didx].offset+1))==NULL) { rc=-1; goto out; }
        sublist[1]=sublist[1]->rbnext;

        /* 9 (text) */
        if ((sublist[1]->rbnext=db_newrb(9,'S',dash[didx].text))==NULL) { rc=-1; goto out; }
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

/*F*/
int db_ltypetabrec::entmod(struct resbuf *modll, db_drawing *pDrawing) 
{
/*
**  Entity/table-record specific.  Uses the setters for range-correction.
**
**  There's some order-dependency here; each 49 signals the beginning
**  of a new set of dash data (and signals the end of the previous set).
**
**  Returns: See db_handitem::entmod().
*/
	int result = 0;
    int nElements = 0;
	int elementIndex;
    struct resbuf *pInitialListItem;
	struct resbuf *pCurrentListItem;
    db_objhandle stylehandle;
    db_ltypedashlink ltypeElement;
    db_lasterror = 0; 
	sds_real	tmplen;
	
    if(modll == NULL || pDrawing == NULL) 
	{ 
		result = -1; 
		return result; 
	}
    del_xdict();
    pInitialListItem = modll;
    /*
    **  If modll has been built properly, the first few groups
    **  are things we don't allow to be modified (ename, type, handle),
    **  so let's walk over these right now:
    */
    while (pInitialListItem != NULL &&
        (pInitialListItem->restype == -1 ||
         pInitialListItem->restype == 0 ||
         pInitialListItem->restype == 5)) 
		 pInitialListItem = pInitialListItem->rbnext;

    // find the number of linetype elements and call set_73() to allocate the dash[] array
    for(pCurrentListItem = pInitialListItem; pCurrentListItem != NULL; pCurrentListItem = pCurrentListItem->rbnext)
        if(pCurrentListItem->restype == 49)
			++nElements;
    if(nElements > DB_MAXLTDASHES) 
		nElements = DB_MAXLTDASHES;
    if(nElements > 0)
		set_73(nElements);
    elementIndex = -1;
    // walk until the end or the EED sentinel (-3): 
    for (pCurrentListItem = pInitialListItem; pCurrentListItem != NULL && pCurrentListItem->restype != -3; pCurrentListItem = pCurrentListItem->rbnext)
	{
        switch (pCurrentListItem->restype) 
		{
            case   3: 
				{
					set_3(pCurrentListItem->resval.rstring); 
					break;
				}
            case   9:
				{
					db_astrncpy(&ltypeElement.text, pCurrentListItem->resval.rstring, -1);
					break;
                }
            case  40: break;
            case  44: 
				{
					ltypeElement.offset[0] = pCurrentListItem->resval.rreal; 
					break;
				}
            case  45: 
				{
					ltypeElement.offset[1] = pCurrentListItem->resval.rreal; 
					break;
				}
            case  46: 
				{
					ltypeElement.scale = pCurrentListItem->resval.rreal;     
					break;
				}
            case  49:
				{
					if (nElements > 0 && elementIndex > -1 && elementIndex < nElements) 
					{
						// process the previous dash data
						stylehandle.Nullify();
						if (ltypeElement.stylehip != NULL)
							stylehandle = ltypeElement.stylehip->RetHandle();
						// EBATECH(CNBR) -[ 2002/4/19 if type==0 ignore rest items
						if( ltypeElement.type == 0 ){
							tmplen = ltypeElement.len;
							delete[] ltypeElement.text;
							ltypeElement.reset();
							stylehandle.Nullify();
							ltypeElement.len = tmplen;
						}
						// EBATECH(CNBR) ]-
					
						set_dashdata(
							elementIndex,
							ltypeElement.text,
							ltypeElement.offset[0],
							ltypeElement.offset[1],
							ltypeElement.scale,
							ltypeElement.len,
							ltypeElement.rot,
							ltypeElement.type,
							ltypeElement.shapecd,
							stylehandle,
							pDrawing);
						delete[] ltypeElement.text;
						ltypeElement.reset();
					}
					ltypeElement.len = pCurrentListItem->resval.rreal;
					++elementIndex;
					break;
				}
            case  50: 
				{
					ltypeElement.rot = pCurrentListItem->resval.rreal; 
					break;
				}
            case  72: break;  // always 'A' 
            case  73: break;  // done earlier, just avoid default case here
            case  74: 
				{
					ltypeElement.type = pCurrentListItem->resval.rint; 
					break;
				}
            case  75: 
				{
					ltypeElement.shapecd = pCurrentListItem->resval.rint;
					break;
				}
            case 340: 
				{
					ltypeElement.stylehip = (db_handitem*)pCurrentListItem->resval.rlname[0];
					break;
				}
            default:  
				// the common entity or table record groups
                result = entmod1common(&pCurrentListItem, pDrawing);
				if(result)
					return result;
        }
    }
    // do the last set of dash data
    if(nElements > 0 && elementIndex > -1 && elementIndex < nElements) 
	{
		stylehandle.Nullify();
		if (ltypeElement.stylehip != NULL) 
			stylehandle = ltypeElement.stylehip->RetHandle();
		// EBATECH(CNBR) -[ 2002/4/19 if type==0 ignore rest items
		if( ltypeElement.type == 0 ){
			tmplen = ltypeElement.len;
			delete[] ltypeElement.text;
			ltypeElement.reset();
			stylehandle.Nullify();
			ltypeElement.len = tmplen;
		}
		// EBATECH(CNBR) ]-
		set_dashdata(
            elementIndex,
            ltypeElement.text,
            ltypeElement.offset[0],
            ltypeElement.offset[1],
            ltypeElement.scale,
            ltypeElement.len,
            ltypeElement.rot,
            ltypeElement.type,
            ltypeElement.shapecd,
            stylehandle,
            pDrawing);
    }
    if(db_lasterror) 
		result = -6;
    // do the EED
    if(!result && pInitialListItem != NULL) 
		result = set_eed(pInitialListItem, pDrawing);
    return result;
}
