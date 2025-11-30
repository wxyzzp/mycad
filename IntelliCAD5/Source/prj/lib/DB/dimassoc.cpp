/* C:\ICAD\PRJ\LIB\DB\DIMASSOC.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "objects.h"

/***************************** db_dimassoc *****************************/

db_dimassoc::db_dimassoc(void) : db_handitem(DB_DIMASSOC) {
	deleted = 0;
	dimensionObjHandle = NULL;
	associativityFlag = 0;
	transSpaceFlag = 0;
	rotatedDimType = 0;
	}
db_dimassoc::db_dimassoc(const db_dimassoc &sour) :
	db_handitem(sour) {  /* Copy constructor */

	deleted = sour.deleted;

	dimensionObjHandle=sour.dimensionObjHandle;
	associativityFlag=sour.associativityFlag;
	transSpaceFlag=sour.transSpaceFlag;
	rotatedDimType=sour.rotatedDimType;
	memcpy(elements,sour.elements,sizeof(elements));
}
db_dimassoc::~db_dimassoc(void) {}

int db_dimassoc::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
/*
**	Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
	int rc, idx, idx2;
	sds_name en;
	struct resbuf *sublist[2];
	char * namep;

	rc=0; sublist[0]=sublist[1]=NULL;

	if (begpp==NULL || endpp==NULL) goto out;

	/* 330	DimensionObjHandle */
	if( !dimensionObjHandle ) {
		en[0] = en[1] = 0;
	}else{
		en[0] = (long)dimensionObjHandle; en[1] = (long)dp;
	}
	if ((sublist[0]=sublist[1]=db_newrb(330,'N',en))==NULL)
		{ rc=-1; goto out; }

	/* 90 Association activity flag */
	if ((sublist[1]->rbnext=db_newrb(90,'I',&associativityFlag))==NULL)
		{ rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	/* 70 Transfer Spaces Flag */
	if ((sublist[1]->rbnext=db_newrb(70,'H',&transSpaceFlag))==NULL)
		{ rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	/* 71 Rotated Dimension Type */
	if ((sublist[1]->rbnext=db_newrb(71,'H',&rotatedDimType))==NULL)
		{ rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	/* 8 times records loop */
	for( idx = 0 ; idx < 8 ; idx++ ) {
		// check elements[i] is complete or not.
		get_1(&namep,idx);
		if( namep == NULL || *namep == '\0' ){
			break;
		}

		/* 1 Pseudo class name */
		if ((sublist[1]->rbnext=db_newrb(1,'S',elements[idx].name))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		/* 72 Object Snap Type */
		if ((sublist[1]->rbnext=db_newrb(72,'H',&elements[idx].osSnapType))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		/* 331 Main object OID */
		if( elements[idx].mainObjHandle ) {
			en[0] = (long)elements[idx].mainObjHandle; en[1] = (long)dp;
		}else{
			en[0] = en[1] = 0;
		}
		if ((sublist[1]->rbnext=db_newrb(331,'N',en))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		/* 73 Main object sub entity type */
		if ((sublist[1]->rbnext=db_newrb(73,'H',&elements[idx].mainSubEntType))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		/* 91 Main object Gs Marker Index */
		if ((sublist[1]->rbnext=db_newrb(91,'I',&elements[idx].mainGsMarker))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		/* 301 Cross Reference Object Handle(multiple) */
		for( idx2 = 0 ; idx2 < elements[idx].xrefHandleCount ; idx2++ ) {

			if( elements[idx].xrefObjHandles[idx2] != NULL ) {
				char asciihandle[17];
				elements[idx].xrefObjHandles[idx2].ToAscii(asciihandle);
				if ((sublist[1]->rbnext=db_newrb(301,'S',asciihandle))==NULL)
					{ rc=-1; goto out; }
				sublist[1]=sublist[1]->rbnext;
			}

		}

		/* 40 Nearest Object Snap Geometry Parameter */
		if ((sublist[1]->rbnext=db_newrb(40,'R',&elements[idx].nearOsnap))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		/* 10 Object Snap Point */
		if ((sublist[1]->rbnext=db_newrb(10,'P',elements[idx].osnapPoint))==NULL)
			{ rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		/* 332 Intersect Point Object OID */
		if( elements[idx].intObjHandle ) {
			en[0] = (long)elements[idx].intObjHandle; en[1] = (long)dp;
			if ((sublist[1]->rbnext=db_newrb(332,'N',en))==NULL)
				{ rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;

			/* 74 Intersect Object Sub Entity Type */
			if ((sublist[1]->rbnext=db_newrb(74,'H',&elements[idx].subEntIntType))==NULL)
				{ rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;

			/* 92 Intersect Object GsMarker Index */
			if ((sublist[1]->rbnext=db_newrb(92,'I',&elements[idx].intGsMarker))==NULL)
				{ rc=-1; goto out; }
			sublist[1]=sublist[1]->rbnext;

			/* 302 Intersect Object Handle */
			if( elements[idx].intXrefObjHandle != NULL ) {
				char asciihandle[17];
				elements[idx].intXrefObjHandle.ToAscii(asciihandle);
				if ((sublist[1]->rbnext=db_newrb(302,'S',asciihandle))==NULL)
					{ rc=-1; goto out; }
				sublist[1]=sublist[1]->rbnext;
			}
		}

		/* 75 Last point reference flag */
		if ((sublist[1]->rbnext=db_newrb(75,'H',&elements[idx].hasLastPointRef))==NULL)
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

