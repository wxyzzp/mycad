/* D:\ICADDEV\PRJ\LIB\DB\INSERT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "objects.h"

/**************************** db_insert *****************************/

/*F*/
db_insert::db_insert(void) : db_entity_with_extrusion(DB_INSERT) {
    pt[0]=pt[1]=pt[2]=colsp=rowsp=rot=0.0;
    scl[0]=scl[1]=scl[2]=1.0; afollow=0; ncols=nrows=1;
    bthip=NULL; 
    disp=NULL; memset(extent,0,sizeof(extent));
	m_pFilter = NULL;
}
db_insert::db_insert(db_drawing *dp) : db_entity_with_extrusion(DB_INSERT,dp) {
    pt[0]=pt[1]=pt[2]=colsp=rowsp=rot=0.0;
    scl[0]=scl[1]=scl[2]=1.0; afollow=0; ncols=nrows=1;
    bthip=NULL;
    disp=NULL; memset(extent,0,sizeof(extent));
	m_pFilter = NULL;
}
db_insert::db_insert(const db_insert &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */


    DB_PTCPY(pt,sour.pt);

    bthip =sour.bthip ; scl[0] =sour.scl[0] ; scl[1]=sour.scl[1];
    scl[2]=sour.scl[2]; colsp  =sour.colsp  ; rowsp =sour.rowsp;
    rot   =sour.rot   ; afollow=sour.afollow; ncols =sour.ncols;
    nrows =sour.nrows ;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
	m_pFilter = sour.m_pFilter;		// I'm not sure about it.
}
db_insert::~db_insert(void) {

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*F*/
int db_insert::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    char *fcp1;
    short fsh1;
    int rc;
    struct resbuf *sublist[2],*trbp1;


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    if (afollow) {
        /* 66 (afollow -- only if non-0, like ACAD) */
        fsh1=(short)afollow;
        if ((sublist[0]=sublist[1]=db_newrb(66,'H',&fsh1))==NULL) { rc=-1; goto out; }
    }

    /* 2 (block name.  CAREFUL!  The 66 above is conditional.) */
    fcp1=NULL;
    if (bthip!=NULL) bthip->get_2(&fcp1);
    if ((trbp1=db_newrb(2,'S',(fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL)
        { rc=-1; goto out; }
    if (sublist[0]==NULL) sublist[0]=sublist[1]=trbp1;
    else { sublist[1]->rbnext=trbp1; sublist[1]=sublist[1]->rbnext; }

    /* 10 (pt) */
    if ((sublist[1]->rbnext=db_newrb(10,'P',pt))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 41 (scl[0]) */
    if ((sublist[1]->rbnext=db_newrb(41,'R',scl))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 42 (scl[1]) */
    if ((sublist[1]->rbnext=db_newrb(42,'R',scl+1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 43 (scl[2]) */
    if ((sublist[1]->rbnext=db_newrb(43,'R',scl+2))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 50 (rot) */
    if ((sublist[1]->rbnext=db_newrb(50,'R',&rot))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 70 (ncols) */
    fsh1=(short)ncols;
    if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 71 (nrows) */
    fsh1=(short)nrows;
    if ((sublist[1]->rbnext=db_newrb(71,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 44 (colsp) */
    if ((sublist[1]->rbnext=db_newrb(44,'R',&colsp))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 45 (rowsp) */
    if ((sublist[1]->rbnext=db_newrb(45,'R',&rowsp))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// Extrusion is now part of the base class, so get it from accessor
	//
	sds_point extru;
	get_210( extru );

    /* 210 (extru) */
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

bool 
db_insert:: set_8(char *p, db_drawing *dp) 
	{
	ASSERT( ret_type() EQ DB_INSERT );
	bool result = db_entity::set_8( p, dp);

	return result;
	}


/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Find clipping spatial_filter object for this insert (if any).
 * Returns:	A pointer to the clipping spatial filter object or NULL if it's not
 *			found.
 ********************************************************************************/
db_handitem*
db_insert::FindFilter
(
 db_drawing*	pDrawing	// =>
)
{
	// Here is the situation.
	// The insert has a reference to some dictionary1 (by xdict field).
	// Dictionary1 has a record named "ACAD_FILTER" which
	// references to another dictionary2 (by hiref field).
	// Dictionary2 has a record named "SPATIAL" which
	// references to the desired SPATIAL_FILTER object (by hiref field).

	db_hireflink*		pXDictionary;
	db_handitem			*pFirstObject, *pLastObject, *pObject = NULL;
	db_dictionaryrec*	pDictRecord;

	ASSERT(pDrawing);
	if(!pDrawing)
		return NULL;

	if(!(pXDictionary = ret_xdict()))
		return NULL;

	pDrawing->get_objllends(&pFirstObject, &pLastObject);

	ASSERT(!pFirstObject == !pLastObject);
	if(!pFirstObject || !pLastObject)
		return NULL;

	// find dictionary1 to which this insert references
	for(pObject = pFirstObject; pObject; pObject = pObject->next)
	{
		if(pObject->ret_type() != DB_DICTIONARY)
			continue;
		if(pObject->RetHandle() == pXDictionary->hand)
			break;
	}

	ASSERT(pObject);
	if(!pObject)
		return NULL;

	// find "ACAD_FILTER" record in the dictionary1
	pDictRecord = ((db_dictionary*)pObject)->findrec("ACAD_FILTER"/*DNT*/, 0, pDrawing);

// Assumes there is always one Not a valid assumption.	ASSERT(pDictRecord);
	if(!pDictRecord)
		return NULL;

	// find dictionary2 to which found "ACAD_FILTER" record references
	for(pObject = pFirstObject; pObject; pObject = pObject->next)
	{
		if(pObject->ret_type() != DB_DICTIONARY)
			continue;
		if(pObject->RetHandle() == pDictRecord->hiref->hand)
			break;
	}

	ASSERT(pObject);
	if(!pObject)
		return NULL;

	// find "SPATIAL" record in the dictionary2
	pDictRecord = ((db_dictionary*)pObject)->findrec("SPATIAL"/*DNT*/, 0, pDrawing);

	ASSERT(pDictRecord);
	if(!pDictRecord)
		return NULL;

	// find the spatial_filter object to which "SPATIAL" record references
	for(pObject = pFirstObject; pObject; pObject = pObject->next)
	{
		if(pObject->ret_type() != DB_SPATIAL_FILTER)
			continue;
		if(pObject->RetHandle() == pDictRecord->hiref->hand)
			break;		// We found it at last!
	}

	return pObject;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Delete attached to the insert spatial filter object and appropriate
 *			dictionaries records and dictionaries themselves from given drawing.
 * Returns:	None.
 ********************************************************************************/
void
db_insert::DeleteFilter
(
 db_drawing* pDrawing	// =>
)
{

	// Look at the comments in db_insert::FindFilter...
	// The strategy for this method: delete things after everything was found.

	db_handitem			*pFirstObject, *pLastObject, *pDictionary1, *pDictionary2;
	db_dictionaryrec	*pFirstRecord, *pLastRecord, *pDictRecord;
	db_hireflink*		pXDictionary;

	ASSERT(pDrawing);
	if(!pDrawing)
		return;

	if(!m_pFilter)
		return;

	// find dictionary1
	if(!(pXDictionary = ret_xdict()))
		return;

	pDrawing->get_objllends(&pFirstObject, &pLastObject);

	ASSERT(!pFirstObject == !pLastObject);
	if(!pFirstObject || !pLastObject)
		return;

	for(pDictionary1 = pFirstObject; pDictionary1; pDictionary1 = pDictionary1->next)
	{
		if(pDictionary1->ret_type() != DB_DICTIONARY)
			continue;
		if(pDictionary1->RetHandle() == pXDictionary->hand)
			break;
	}

	ASSERT(pDictionary1);
	if(!pDictionary1)
		return;

	// find "ACAD_FILTER" record
	pDictRecord = ((db_dictionary*)pDictionary1)->findrec("ACAD_FILTER"/*DNT*/, 0, pDrawing);

	ASSERT(pDictRecord);
	if(!pDictRecord)
		return;

	// find dictionary2
	for(pDictionary2 = pFirstObject; pDictionary2; pDictionary2= pDictionary2->next)
	{
		if(pDictionary2->ret_type() != DB_DICTIONARY)
			continue;
		if(pDictionary2->RetHandle() == pDictRecord->hiref->hand)
			break;
	}

	ASSERT(pDictionary2);
	if(!pDictionary2)
		return;

	// find "SPATIAL" record
	pDictRecord = ((db_dictionary*)pDictionary2)->findrec("SPATIAL"/*DNT*/, 0, pDrawing);

	ASSERT(pDictRecord);
	if(!pDictRecord)
		return;

	// delete these records
	((db_dictionary*)pDictionary1)->delrec("ACAD_FILTER"/*DNT*/);
	((db_dictionary*)pDictionary2)->delrec("SPATIAL"/*DNT*/);

	// delete dictionary1 if necessary
	((db_dictionary*)pDictionary1)->get_recllends(&pFirstRecord, &pLastRecord);

	ASSERT(!pFirstRecord == !pLastRecord);
	if(!pFirstRecord && !pLastRecord)
		pDrawing->delhanditem(NULL, pDictionary1, 3);

	// delete dictionary2 if necessary
	((db_dictionary*)pDictionary2)->get_recllends(&pFirstRecord, &pLastRecord);

	ASSERT(!pFirstRecord == !pLastRecord);
	if(!pFirstRecord && !pLastRecord)
		pDrawing->delhanditem(NULL, pDictionary2, 3);

	// delete xdict hireflink
	del_xdict();

	// delete spatial filter object
	pDrawing->delhanditem(NULL, m_pFilter, 3);

	m_pFilter = NULL;
}

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Create new clipping spatial_filter object for this insert.
 *			Note, 1st, DeleteFilter method is called here.
 * Returns:	A pointer to the created clipping spatial filter object or NULL in
 *			failure.
 ********************************************************************************/
db_handitem*
db_insert::CreateNewFilter
(
 db_drawing*	pDrawing	// =>
)
{
	// Look at the comments in db_insert::FindFilter...

	db_hireflink*		pXDictionary;
	db_handitem			*pFirstObject, *pLastObject,
						*pDictionary1 = NULL, *pDictionary2 = NULL, *pSpatialFilter = NULL;
	db_objhandle		Handle;
	ReactorSubject*		pReactor;

	ASSERT(pDrawing);
	if(!pDrawing)
		return NULL;

	DeleteFilter(pDrawing);

	// create dictionary1 if it's not exist
	if(pXDictionary = ret_xdict())
	{
		pDrawing->get_objllends(&pFirstObject, &pLastObject);

		ASSERT(!pFirstObject == !pLastObject);
		if(!pFirstObject || !pLastObject)
			return NULL;

		for(pDictionary1 = pFirstObject; pDictionary1; pDictionary1 = pDictionary1->next)
		{
			if(pDictionary1->ret_type() != DB_DICTIONARY)
				continue;
			if(pDictionary1->RetHandle() == pXDictionary->hand)
				break;
		}
	}

	if(!pDictionary1)
	{
		if(!(pDictionary1 = new db_dictionary))
			return NULL;

		if(pDrawing->addhanditem(pDictionary1))
		{
			delete pDictionary1;
			return NULL;
		}

		Handle = pDictionary1->RetHandle();

		set_xdict(DB_SOFT_POINTER, Handle, NULL);
	}

	// dictionary1 has reactor to the insert
	pReactor = pDictionary1->getReactor();

	ASSERT(pReactor);
	if(!pReactor)
	{
		if(!pDrawing->delhanditem(NULL, pDictionary1, 3))
			delete pDictionary1;
		return NULL;
	}

	pReactor->AttachReactor(RetHandle());

	// allocate dictionary2 & spatial_filter
	if(!(pDictionary2 = new db_dictionary))
	{
		if(!pDrawing->delhanditem(NULL, pDictionary1, 3))
			delete pDictionary1;
		return NULL;
	}

	if(!(pSpatialFilter = new db_spatial_filter))
	{
		if(!pDrawing->delhanditem(NULL, pDictionary1, 3))
			delete pDictionary1;
		delete pDictionary2;
		return NULL;
	}

	// add dictionary2 & spatial_filter to the database
	if(pDrawing->addhanditem(pDictionary2))
	{
		if(!pDrawing->delhanditem(NULL, pDictionary1, 3))
			delete pDictionary1;
		delete pDictionary2;
		delete pSpatialFilter;
		return NULL;
	}

	if(pDrawing->addhanditem(pSpatialFilter))
	{
		if(!pDrawing->delhanditem(NULL, pDictionary1, 3))
			delete pDictionary1;
		if(!pDrawing->delhanditem(NULL, pDictionary2, 3))
			delete pDictionary2;
		delete pSpatialFilter;
		return NULL;
	}

	// dictionary2 has reactor to dictionary1
	pReactor = pDictionary2->getReactor();

	ASSERT(pReactor);

	pReactor->AttachReactor(pDictionary1->RetHandle());

	// spatial_filter has reactor to dictionary2
	pReactor = pSpatialFilter->getReactor();

	ASSERT(pReactor);

	pReactor->AttachReactor(pDictionary2->RetHandle());

	// add ACAD_FILTER record to the dictionary1
	if(pDrawing->dictadd(pDictionary1, "ACAD_FILTER"/*DNT*/, pDictionary2, DB_HARD_OWNER))
		ASSERT(0);

	// add SPATIAL record to the dictionary2
	if(pDrawing->dictadd(pDictionary2, "SPATIAL"/*DNT*/, pSpatialFilter, DB_HARD_OWNER))
		ASSERT(0);

	m_pFilter = pSpatialFilter;

	return pSpatialFilter;
}
