/* C:\ICAD\PRJ\LIB\DB\BLOCKTABREC.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/************************** db_blocktabrec **************************/

/*F*/

db_blocktabrec::db_blocktabrec(void) :
    db_tablerecord(DB_BLOCKTABREC,db_str_quotequote) {

    entll[0]=entll[1]=oldentll[0]=oldentll[1]=NULL;
	description[0]=0;
	layoutobjhandle=NULL;
	}
db_blocktabrec::db_blocktabrec(char *pname, db_drawing *dp) :
    db_tablerecord(DB_BLOCKTABREC,pname) {

    /* Do FIRST (since set_2() looks at entll[0]): */
    entll[0]=entll[1]=oldentll[0]=oldentll[1]=NULL;
    db_blocktabrec::set_2(pname,dp);// VT: Fix. 21-3-02. VMT may be initialized after construction
	description[0]=0;
	layoutobjhandle=NULL;
}
db_blocktabrec::db_blocktabrec(const db_blocktabrec &sour) :
    db_tablerecord(sour) {  /* Copy constructor */

    /* Don't copy the block def: */
    entll[0]=entll[1]=oldentll[0]=oldentll[1]=NULL;
	strncpy(description,sour.description,sizeof(description));
	layoutobjhandle=NULL;
}
db_blocktabrec::~db_blocktabrec(void) {
    delll(entll[0]); delll(oldentll[0]);
}


/* Here are the ones for db_block and db_blocktabrec for the data they */
/* double-handle (name and flags): */

inline	bool db_blocktabrec::set_2(char* p) {
	    db_astrncpy(&m_pName,(p==NULL) ? db_str_quotequote : p,IC_ACADNMLEN-1);
		if (entll[0]!=NULL && entll[0]->ret_type()==DB_BLOCK)
			db_astrncpy(&((db_block *)entll[0])->name,m_pName,-1);
	    return true;
	}

inline	bool db_blocktabrec::set_70(int p) {
	    m_flags=(char)p;
		if (entll[0]!=NULL && entll[0]->ret_type()==DB_BLOCK)
			((db_block *)entll[0])->tflags=m_flags;
	    return true;
	}

void db_blocktabrec::addent(db_handitem* p, db_drawing* dp)
{
	/*DG - 29.1.2002*/// See comments below for purposes of the var.
	// I made it static and therefore the method outline because it's used only here now.
	static db_polyline*	pPolylineAdded = NULL;

	if(!p)
		return;

	if(!entll[0])
	{
		if(DB_BLOCK == p->ret_type())
			((db_block*)p)->set_bthip(this);
	    entll[0] = p;
	}
	else
		entll[1]->next = p;
	entll[1] = p;

	// Add an element to handleTable
	dp->AddHandleToTable(p);

	// set paren block to added entity
	p->m_parentBlock = this;

	/*DG - 29.1.2002*/// When we add vertices to a drawing we set back pointers to polylines in db_drawing::addhanditem.
	// Now, when we are adding ents to the block do the analogous things.
	if(p->ret_type() == DB_POLYLINE)
		pPolylineAdded = static_cast<db_polyline*>(p);
	else
		if(p->ret_type() == DB_VERTEX)
			((db_vertex*)p)->setParentPolyline(pPolylineAdded);
		else
			pPolylineAdded = NULL;
}

/*DG - 27.4.2002*/// added the method
void db_blocktabrec::delent(db_handitem* p, db_drawing* dp)
{
	// NOTE, the method doesn't delete *p, it just removes it from entll.

	if(p == entll[0])
	{
		p->m_parentBlock = NULL;
		entll[0] = p->next;
		if(p == entll[1])
			entll[1] = p->next;
		return;
	}

	for(db_handitem* pCurItem = entll[0]; pCurItem && pCurItem->next != p; pCurItem = pCurItem->next)
		;
	
	if(pCurItem)
	{
		p->m_parentBlock = NULL;
		pCurItem->next = pCurItem->next->next;
		if(entll[1] == p)
			entll[1] = pCurItem;
	}
}


#ifdef OLDVERSION
bool db_blocktabrec::get_firstentinblock(sds_name p, db_drawing *dp) {
	db_handitem *thip1;

	/* Skip the BLOCK entity and get to the 1st ent in the def: */
	for (thip1=entll[0]; thip1!=NULL &&
		thip1->ret_type()==DB_BLOCK; thip1=thip1->next);
		p[0]=(long)thip1; p[1]=(long)dp;
		return true;
}
bool db_blocktabrec::get_firstentinblock(db_handitem **p) {
	db_handitem *thip1;

	/* Skip the BLOCK entity and get to the 1st ent in the def: */
	for (thip1=entll[0]; thip1!=NULL &&
		thip1->ret_type()==DB_BLOCK; thip1=thip1->next);
	*p=thip1; return true;
}
#endif

// NEW VERSION
inline bool db_blocktabrec::get_firstentinblock(sds_name p, db_drawing *dp) {
	db_handitem *thip1;
	db_drawing *retdp=dp;
	db_drawing *xrefdp;

	thip1=entll[0];
	if (thip1!=NULL &&  thip1->ret_type()==DB_BLOCK) {
		if (((db_block *)thip1)->ret_flags() & IC_BLOCK_XREF) {
			xrefdp=((db_block *)thip1)->ret_xrefdp();
			if (xrefdp!=NULL) {
				retdp=xrefdp;
				thip1=retdp->ret_firstent();
			}
			else {
				retdp=NULL; thip1=NULL;
			}
		}
		else {
			thip1=thip1->next;
		}
    }
	p[0]=(long)thip1; p[1]=(long)retdp;
	return true;
}

inline bool db_blocktabrec::get_firstentinblock(db_handitem **p) {
	db_handitem *thip1;
	db_drawing *xrefdp;

	thip1=entll[0];
	if (thip1!=NULL && thip1->ret_type()==DB_BLOCK) {
		if (((db_block *)thip1)->ret_flags() & IC_BLOCK_XREF) {
			xrefdp=((db_block *)thip1)->ret_xrefdp();
			if (xrefdp!=NULL) {
				thip1=xrefdp->ret_firstent();
			}
			else {
				thip1=NULL;
			}
		}
		else {
			thip1=thip1->next;
		}
	}
	*p=thip1; return true;
}
// END NEW VERSION




int db_blocktabrec::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/

    // VT: Fix. 21-3-02: initialize variables before use
    int rc = 0;
    sds_name en;
    struct resbuf *sublist[2], *temprb;

    sublist[0] = sublist[1] = NULL;  // VT: Fix. 21-3-02
    if (begpp==NULL || endpp==NULL) goto out;

	//Get general info from db_handitem.
	db_handitem::entgetspecific(sublist, sublist + 1, dp);


    /* Note that for BLOCK, ALL values come from the table record. */

    /* -2 (ename of 1st ent in def) */
    get_firstentinblock(en,dp);
	temprb = db_newrb(-2,'N',en);

	if (temprb == NULL) 
		{  
			rc=-1; 
			goto out; 
		}

	if (sublist[0] && sublist[1])
	{
		sublist[1]->rbnext = temprb;
		sublist[1]=sublist[1]->rbnext;
	}
	else
	{
		sublist[0]=sublist[1] = temprb;
	}

out:
    if (begpp!=NULL && endpp!=NULL) {
        if (rc && sublist[0]!=NULL)
            { sds_relrb(sublist[0]); sublist[0]=sublist[1]=NULL; }
        *begpp=sublist[0]; *endpp=sublist[1];
    }
    return rc;
}

int db_blocktabrec::check(db_drawing* pDrawing)
{
	if(!db_handitem::check(pDrawing))
		return 0;

	db_handitem* pElement;
	for(pElement = entll[0]; pElement; pElement = pElement->next)
		if(!pElement->check(pDrawing)) // currently it can't fail so just assert
			assert(false);

	return 1;
}


