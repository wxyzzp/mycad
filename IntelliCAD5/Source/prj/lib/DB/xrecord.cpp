/* C:\ICAD\PRJ\LIB\DB\XRECORD.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "objects.h"

/***************************** db_xrecord *****************************/

db_xrecord::db_xrecord(void) : db_handitem(DB_XRECORD) 
{
    deleted=0;
	m_pResbufChain = NULL;
}

db_xrecord::db_xrecord(const db_xrecord &sour) :
    db_handitem(sour) 
{  /* Copy constructor */

    deleted=sour.deleted;

	m_pResbufChain = sour.m_pResbufChain;
}
db_xrecord::~db_xrecord(void) 
{ 
	if (m_pResbufChain != NULL)
		sds_relrb(m_pResbufChain);
}

int db_xrecord::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) 
{
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    int rc;
	resbuf *rb;
    struct resbuf *sublist[2];

	struct sds_resbuf* tmp = (struct sds_resbuf *)getResbufChain();

    rc=0; sublist[0] = sublist[1] = NULL;
    if (begpp==NULL || endpp==NULL) 
		goto out;

	if (tmp == NULL)
	{
		rc=-__LINE__;
		goto out;
	}

	while (tmp)
	{
		switch(ic_resval(tmp->restype))
		{
		case RTSTR:
			rb = db_newrb(tmp->restype,'S',tmp->resval.rstring);
			break;
		case RT3DPOINT:
			rb = db_newrb(tmp->restype,'P',tmp->resval.rpoint);
			break;
		case RTSHORT:
			rb = db_newrb(tmp->restype,'I',&tmp->resval.rint);
			break;
		case RTLONG:
			rb = db_newrb(tmp->restype,'L',&tmp->resval.rlong);
			break;
		case RTREAL:
			rb = db_newrb(tmp->restype,'R',&tmp->resval.rreal);
			break;
		case RTBINARY:
			rb = db_newrb(tmp->restype,'B',(void*)&tmp->resval.rbinary);
			break;
		case RTENAME:
			rb = db_newrb(tmp->restype,'N',tmp->resval.rlname);	
			break;
		default:
			ASSERT(!"FALSE");
			break;
		}

		if (rb != NULL)
		{
			if (sublist[0] == NULL)
				sublist[0] = sublist[1] = rb;
			else	// not the first one
			{
				sublist[1]->rbnext = rb;
				sublist[1] = sublist[1]->rbnext;
			}
		}

		tmp = tmp->rbnext;
	}
out:
    if (begpp!=NULL && endpp!=NULL) 
	{
        if (rc && sublist[0]!=NULL)
		{ 
			sds_relrb(sublist[0]); 
			sublist[0] = sublist[1] = NULL; 
		}
        *begpp=sublist[0]; 
		*endpp=sublist[1];
    }

    return rc;
}

int db_xrecord::entmod(struct resbuf *modll, db_drawing *dp) 
{
/*
**  Entity/table-record/object specific.  Uses the setters for range-correction.
**
**  Returns: See db_handitem::entmod().
*/
    int rc;
    struct resbuf *curlink;
    db_handitem *thiship;
	short rbtype;

    rc=0; 
	thiship=(db_handitem *)this;
    db_lasterror=0;

    if (modll==NULL || dp==NULL) 
	{ 
		rc=-1; 
		goto out; 
	}

    del_xdict();
    curlink=modll;

	resbuf *rb;
	struct resbuf *sublist[2];
	sublist[0] = sublist[1] = NULL;

    /*
    **  If modll has been built properly, the first few groups
    **  are things we don't allow to be modified (ename, type, handle),
    **  so let's walk over these right now:
    */
    while (curlink!=NULL &&
          (curlink->restype==-1 ||
           curlink->restype== 0 ||
           curlink->restype== 5 ||
		   curlink->restype==102))
		{
		if (curlink->restype==102)
			{
			curlink=curlink->rbnext;
			while (curlink->restype!=102)
				curlink=curlink->rbnext;
			}
		curlink=curlink->rbnext;
		}

    /* Walk; stop at the end or the EED sentinel (-3): */
    for (; !rc && curlink!=NULL && curlink->restype!=-3; curlink=curlink->rbnext) 
	{
        /* Call the correct setter: */
		rbtype=curlink->restype;
		if (rbtype==100)
			continue; // don't bother with this, and certainly don't store as a string

		switch(ic_resval(curlink->restype))
		{
		case RTSTR:
			rb = db_newrb(curlink->restype,'S',curlink->resval.rstring);
			rc = -__LINE__;
			break;
		case RT3DPOINT:
			rb = db_newrb(curlink->restype,'P',curlink->resval.rpoint);
			rc = -__LINE__;
			break;
		case RTSHORT:
			rb = db_newrb(curlink->restype,'I',&curlink->resval.rint);
			rc = -__LINE__;
			break;
		case RTLONG:
			rb = db_newrb(curlink->restype,'L',&curlink->resval.rlong);
			rc = -__LINE__;
			break;
		case RTREAL:
			rb = db_newrb(curlink->restype,'R',&curlink->resval.rreal);
			rc = -__LINE__;
			break;
		case RTBINARY:
			rb = db_newrb(curlink->restype,'B',(void*)&curlink->resval.rbinary);
			rc = -__LINE__;
			break;
		case RTENAME:
			rb = db_newrb(curlink->restype,'N',curlink->resval.rlname);	
			rc = -__LINE__;
			break;
		default:
			ASSERT(!"FALSE");
			rc = -__LINE__;
			break;
		}

		if (rb != NULL)
		{
			rc = 0;
			if (sublist[0] == NULL)
				sublist[0] = sublist[1] = rb;
			else	// not the first one
			{
				sublist[1]->rbnext = rb;
				sublist[1] = sublist[1]->rbnext;
			}
		}
		else
		{
			goto errorout;
		}
	}

    if (db_lasterror) 
	{ 
		rc=-6; 
		goto out; 
	}

    if (!rc && curlink!=NULL) 
		rc=set_eed(curlink,dp);

out:
	if (m_pResbufChain != NULL)
		sds_relrb(m_pResbufChain);
	setResbufChain(sublist[0]);	// Destructor will delete these.
    return rc;
errorout:
	sds_relrb(sublist[0]);
	return rc;
	}


