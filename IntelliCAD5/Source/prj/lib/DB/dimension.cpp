/* C:\ICAD\PRJ\LIB\DB\DIMENSION.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "DbMText.h"

/************************** db_dimension *****************************/

/*F*/
db_dimension::db_dimension(void) : db_entity_with_extrusion(DB_DIMENSION) {
    dimtext=NULL; bthip=dship=NULL; memset(pt,0,sizeof(pt));
    leadlen=ang=hdir=slant=textrot=0.0; dimtype=0; insrot=0.0;
    disp=NULL; memset(extent,0,sizeof(extent));
}
db_dimension::db_dimension(db_drawing *dp) : db_entity_with_extrusion(DB_DIMENSION,dp) {

    dimtext=NULL; bthip=NULL;
    dship=(dp!=NULL) ? dp->ret_currenttabrec(DB_QDIMSTYLE) : NULL;
    memset(pt,0,sizeof(pt));
    leadlen=ang=hdir=slant=textrot=0.0; dimtype=0; insrot=0.0;


    disp=NULL; memset(extent,0,sizeof(extent));
}
db_dimension::db_dimension(const db_dimension &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */


    dimtext=NULL; if (sour.dimtext!=NULL) db_astrncpy(&dimtext,sour.dimtext,-1);

    DB_PTCPY(pt[0],sour.pt[0]); DB_PTCPY(pt[1],sour.pt[1]);
    DB_PTCPY(pt[2],sour.pt[2]); DB_PTCPY(pt[3],sour.pt[3]);
    DB_PTCPY(pt[4],sour.pt[4]); DB_PTCPY(pt[5],sour.pt[5]);
    DB_PTCPY(pt[6],sour.pt[6]);

    bthip  =sour.bthip  ; dship  =sour.dship;   leadlen=sour.leadlen;
    ang    =sour.ang    ; hdir   =sour.hdir;    slant  =sour.slant  ;
    textrot=sour.textrot; dimtype=sour.dimtype; insrot =sour.insrot;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_dimension::~db_dimension(void) {
    delete [] dimtext; 

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_dimension::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    char *fcp1;
    short fsh1;
    int rc;
    struct resbuf *sublist[2],wcsrb,ecsrb;


    rc=0; sublist[0]=sublist[1]=NULL;

    wcsrb.rbnext=ecsrb.rbnext=NULL;
    wcsrb.restype=RTSHORT; ecsrb.restype=RT3DPOINT;
    wcsrb.resval.rint=0;

	get_210( ecsrb.resval.rpoint );

    if (begpp==NULL || endpp==NULL) goto out;

    /* 2 (block name) */
    fcp1=NULL;
    if (bthip!=NULL) bthip->get_2(&fcp1);
    if ((sublist[0]=sublist[1]=db_newrb(2,'S',
        (fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL) { rc=-1; goto out; }

    /* 10 (pt[0]) */
    if ((sublist[1]->rbnext=db_newrb(10,'P',pt[0]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 11 (pt[1]) */
    if ((sublist[1]->rbnext=db_newrb(11,'P',pt[1]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 12 (pt[2]) (Needs to be WCS to match ACAD.) */
    if ((sublist[1]->rbnext=db_newrb(12,'P',pt[2]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;
    dp->trans(sublist[1]->resval.rpoint,&ecsrb,&wcsrb,0,sublist[1]->resval.rpoint,NULL);

    /* 70 (dimtype) */
    fsh1=(short)dimtype;
    if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 1 (dimtext) */
    if ((sublist[1]->rbnext=db_newrb(1,'S',
        (dimtext!=NULL) ? dimtext : db_str_quotequote))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 52 (slant) */
    if ((sublist[1]->rbnext=db_newrb(52,'R',&slant))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 53 (textrot) */
    if ((sublist[1]->rbnext=db_newrb(53,'R',&textrot))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 54 (insrot) */
    if ((sublist[1]->rbnext=db_newrb(54,'R',&insrot))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 51 (hdir) */
    if ((sublist[1]->rbnext=db_newrb(51,'R',&hdir))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// Extrusion is now part of the base class, so get it from accessor
	//
	sds_point extru;
	get_210( extru );

    /* 210 (extru) */
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 3 (dimstyle name) */
    fcp1=NULL;
    if (dship!=NULL) dship->get_2(&fcp1);
    if ((sublist[1]->rbnext=db_newrb(3,'S',
        (fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 13 (pt[3]) */
    if ((sublist[1]->rbnext=db_newrb(13,'P',pt[3]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 14 (pt[4]) */
    if ((sublist[1]->rbnext=db_newrb(14,'P',pt[4]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 15 (pt[5]) */
    if ((sublist[1]->rbnext=db_newrb(15,'P',pt[5]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 16 (pt[6]) */
    if ((sublist[1]->rbnext=db_newrb(16,'P',pt[6]))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 40 (leadlen) */
    if ((sublist[1]->rbnext=db_newrb(40,'R',&leadlen))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	//Bugzilla No. 78310 ; 07-10-2002
	/* 42 (measurement) */
    if ((sublist[1]->rbnext=db_newrb(42,'R',&measurement))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 50 (ang) */
    if ((sublist[1]->rbnext=db_newrb(50,'R',&ang))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

/*F*/
int db_dimension::entmod(struct resbuf *modll, db_drawing *dp) {
/*
**  Entity/table-record specific.  Uses the setters for range-correction.
**
**  Needed a specific one so that we can catch the extrusion first
**  and use it for the WCS->ECS transformation of the 12-point.
**
**  Returns: See db_handitem::entmod().
*/
    int rc;
    sds_point ap1;
    struct resbuf *startlink,*curlink,wcsrb,ecsrb;
    db_handitem *thiship;


    rc=0; thiship=(db_handitem *)this;

    db_lasterror=0;

    if (modll==NULL || dp==NULL) { rc=-1; goto out; }

    /* Set up wcsrb and ecsrb except for ecsrb.resval.rpoint: */
    wcsrb.rbnext=ecsrb.rbnext=NULL;
    wcsrb.restype=RTSHORT; ecsrb.restype=RT3DPOINT;
    wcsrb.resval.rint=0;

    del_xdict();

    startlink=modll;

    /*
    **  If modll has been built properly, the first few groups
    **  are things we don't allow to be modified (ename, type, handle),
    **  so let's walk over these right now:
    */
    while (startlink!=NULL &&
        (startlink->restype==-1 ||
         startlink->restype== 0 ||
         startlink->restype== 5)) startlink=startlink->rbnext;

    /* We need the extrusion before we do anything else (for transformations */
    /* involving the ECS), so set it first: */
    for (curlink=startlink; curlink!=NULL &&
        curlink->restype!=210; curlink=curlink->rbnext);
    if (curlink!=NULL) thiship->set_210(curlink->resval.rpoint);

    /* Now set ecsrb.resval.rpoint to whatever extru is (the val just set -- */
    /* or the existing one if never changed): */
	get_210( ecsrb.resval.rpoint );

    /* Walk; stop at the end or the EED sentinel (-3): */
    for (curlink=startlink; !rc && curlink!=NULL &&
        curlink->restype!=-3; curlink=curlink->rbnext) {

        /* Call the correct setter: */
        switch (curlink->restype) {
            case   1: thiship->set_1(curlink->resval.rstring); break;
            case   2: thiship->set_2(curlink->resval.rstring,dp); break;
            case   3: thiship->set_3(curlink->resval.rstring,dp); break;
            case  10: thiship->set_10(curlink->resval.rpoint); break;
            case  11: thiship->set_11(curlink->resval.rpoint); break;
            case  12: /* Here's the special one.  Transform WCS->ECS first. */
                dp->trans(curlink->resval.rpoint,&wcsrb,&ecsrb,0,ap1,NULL);
                thiship->set_12(ap1);
                break;
            case  13: thiship->set_13(curlink->resval.rpoint); break;
            case  14: thiship->set_14(curlink->resval.rpoint); break;
            case  15: thiship->set_15(curlink->resval.rpoint); break;
            case  16: thiship->set_16(curlink->resval.rpoint); break;
            case  40: thiship->set_40(curlink->resval.rreal);  break;
            case  50: thiship->set_50(curlink->resval.rreal);  break;
            case  51: thiship->set_51(curlink->resval.rreal);  break;
            case  52: thiship->set_52(curlink->resval.rreal);  break;
            case  53: thiship->set_53(curlink->resval.rreal);  break;
            case  54: thiship->set_54(curlink->resval.rreal);  break;
            case  70: thiship->set_70(curlink->resval.rint);   break;

            case 210: break;  /* Did it earlier. */

            default:  /* The common entity or table record groups. */
                rc=thiship->entmod1common(&curlink,dp);
                break;
        }
    }

    if (db_lasterror) { rc=-6; goto out; }

    /* Do EED: */
    if (!rc && curlink!=NULL) rc=set_eed(curlink,dp);

out:
    return rc;
}

/*****************************************************************/
// Given a dimension, find the handle of the associated text or mtext entity.
// If successful, exactly one of pText or pMtext will be non-null.
// The assumption is made that there is at most one text or mtext entity
// associated with the dimension.
bool db_dimension::get_texthandle(db_text * &pText, CDbMText * &pMtext)
	   
	{
	bool found = false;
	pText = NULL;
	pMtext = NULL;

	// Get the handle to the block associated with the dimension.

	db_handitem * pBlock;

	get_2( &pBlock);

	// Walk through the entities in this block until we reach a text or mtext entity.
	db_handitem * pCurr;
	((db_blocktabrec *)pBlock)->get_firstentinblock(&pCurr);
	bool done = false;
	while (!done)
		{   // begin while !done
		if (pCurr == NULL)
			done = true;
		else
			{  // begin case of pCurr != NULL
			switch (pCurr->ret_type())
				{
				case DB_TEXT:
					pText = (db_text *) pCurr;
					found = true;
					done = true;
					break;
				case DB_MTEXT:
					pMtext = (CDbMText *) pCurr;
					found = true;
					done = true;
					break;
				default:
					// no action taken here
					break;
				}
			pCurr = pCurr->next;
			}  //   end case of pCurr != NULL
		}   //   end while !done

	return found;
	}





/*****************************************************************/


