/* C:\ICAD\PRJ\LIB\DB\DICTIONARYWDFLT.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "objects.h"

/***************************** db_dictionarywdflt *****************************/

db_dictionarywdflt::db_dictionarywdflt(void) : db_handitem(DB_DICTIONARYWDFLT) {
	deleted = 0;	/*DG - 5.6.2002*/// obviously necessary
	numitems=0;
	cloningflag=0;
	recll[0]=recll[1]=NULL;
	defaultobject=NULL;
	acadnextrec=icadnextrec=NULL;
	}
db_dictionarywdflt::db_dictionarywdflt(const db_dictionarywdflt &sour) :
	db_handitem(sour) {  /* Copy constructor */


	deleted = sour.deleted;	/*DG - 5.6.2002*/// obviously necessary

	numitems=sour.numitems;

	cloningflag=sour.cloningflag;
	recll[0]=recll[1]=NULL; // for now we do not copy the items
	defaultobject=sour.defaultobject;
	acadnextrec=icadnextrec=NULL;

	// 2002/9/6 Ebatech(cnbr) -[ copy new all records
	db_dictionaryrec *tp1,*tp2;
	for (tp1=sour.recll[0]; tp1!=NULL; tp1=tp1->next) {
		tp2=new db_dictionaryrec(*tp1);
		addrec(tp2);
		if (sour.acadnextrec==tp1) acadnextrec=tp2;
		if (sour.icadnextrec==tp1) icadnextrec=tp2;
	}
	// Ebatech(cnbr) ]-
}
db_dictionarywdflt::~db_dictionarywdflt(void) { delrecll(); }

// EBATECH(CNBR) -[ 2002/6/30 Bugzilla78227
int db_dictionarywdflt::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
/*
**	Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
	short fsh1;
	int rc;
	sds_name en;
	struct resbuf *sublist[2];
	db_handitem *thip1;
	db_dictionaryrec *tdrp1;


	rc=0; sublist[0]=sublist[1]=NULL;

	if (begpp==NULL || endpp==NULL) goto out;

	/* 281	Cloning flag */
	fsh1 = cloningflag;
	if ((sublist[0]=sublist[1]=db_newrb(281,'H',&fsh1))==NULL)
		{ rc=-1; goto out; }

	/* The 3 and 350 pairs: */
	for (tdrp1=recll[0]; tdrp1!=NULL; tdrp1=tdrp1->next) {
		if ((sublist[1]->rbnext=db_newrb(3,'S',tdrp1->name))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		en[0]=en[1]=0;
		if (tdrp1->hiref!=NULL && (thip1=tdrp1->hiref->ret_hip(dp))!=NULL)
			{ en[0]=(long)thip1; en[1]=(long)dp; }
		if ((sublist[1]->rbnext=db_newrb(350,'N',en))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

	/* 340	Default object */
	en[0]=en[1]=0;
	if( defaultobject )
		{ en[0]=(long)defaultobject; en[1]=(long)dp; }
	if ((sublist[1]->rbnext=db_newrb(340,'N',en))==NULL)
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

int db_dictionarywdflt::entmod(struct resbuf *modll, db_drawing *dp)
{
/*
**	Entity/table-record/object specific.  Uses the setters for range-correction.
**
**	Returns: See db_handitem::entmod().
*/
	int didrecs,rc;
	struct resbuf *curlink;
	db_handitem *thiship,*thip1;
	db_dictionaryrec *tdrp1;


	rc=didrecs=0; thiship=(db_handitem *)this;

	db_lasterror=0;

	if (modll==NULL || dp==NULL) { rc=-1; goto out; }

	del_xdict();

	curlink=modll;

	/* Free the old llist of records (so that the caller can delete */
	/* ALL records by not supplying any in modll): */
	delrecll();

	/*
	**	If modll has been built properly, the first few groups
	**	are things we don't allow to be modified (ename, type, handle),
	**	so let's walk over these right now:
	*/
	while (curlink!=NULL &&
		  (curlink->restype==-1 ||
		   curlink->restype== 0 ||
		   curlink->restype== 5)) curlink=curlink->rbnext;

	/* Walk; stop at the end or the EED sentinel (-3): */
	for (; !rc && curlink!=NULL && curlink->restype!=-3; curlink=curlink->rbnext) {
		/* Call the correct setter: */
		switch (curlink->restype) {
			case   3:
				/* Do the records (the 3/350 pairs).  They MUST come in */
				/* that order: 3,350,3,350,... . */

				if (didrecs) break;

				didrecs=1;

// probably not just 350 and 360 here (I just added 360) but perhaps others also
				while (curlink->restype==3 && curlink->rbnext!=NULL &&
					(curlink->rbnext->restype==350 || curlink->rbnext->restype==360)) {
					int linktype;

					if (curlink->rbnext->restype==350) linktype=DB_SOFT_OWNER;
					else if (curlink->rbnext->restype==360) linktype=DB_HARD_OWNER;

					thip1=(db_handitem *)curlink->rbnext->resval.rlname[0];

					ASSERT(thip1);
					/* Make the new link and add it on: */
					tdrp1=new db_dictionaryrec(
						curlink->resval.rstring,
						linktype,
						thip1->RetHandle(),
						thip1);
					addrec(tdrp1);

					curlink=curlink->rbnext;  /* Step to 350 */
					/* Step to next 3 (if there is one): */
					if (curlink->rbnext!=NULL && curlink->rbnext->restype==3)
						curlink=curlink->rbnext;
				}

				break;
			case 350: break;  /* Handled in case 3. */
			case 360: break;  // same
			//case 280: break;	/* Not in our database (or MarComp's, apparently). */
			case 281:
				set_281(curlink->resval.rint);
				break;
			case 340:
				set_340((db_handitem*)curlink->resval.rlname[0]);
				break;
			default:  /* In the case of an OBJECT, this virtual function */
					  /* calls entmodhi(). */
				rc=thiship->entmod1common(&curlink,dp);
				break;
		}
	}

	if (db_lasterror) { rc=-6; goto out; }

	if (!rc && curlink!=NULL) rc=set_eed(curlink,dp);

out:

	return rc;
}
// EBATECH(CNBR) ]-
