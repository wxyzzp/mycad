/* C:\ICAD\PRJ\LIB\DB\ACAD_PROXY_OBJECT.CPP
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
#include "DbProxyObject.h"

/***************************** db_acad_proxy_object *****************************/

db_acad_proxy_object::db_acad_proxy_object(void) : db_handitem(DB_ACAD_PROXY_OBJECT) 
{
	deleted=0;
	hirefll[0]=hirefll[1]=NULL;
	aid=0;
	pid=499; objectdrawingformat=origdataformat=0;
	// EBATECH(CNBR) ]-
	m_pResbufChain = NULL;
	m_OdClass  = NULL;
}

db_acad_proxy_object::db_acad_proxy_object(const db_acad_proxy_object &sour) :
	db_handitem(sour) 
{  /* Copy constructor */

	db_hireflink *tp1,*tp2;

	deleted=sour.deleted;

	pid=sour.pid; aid=sour.aid; 
	objectdrawingformat=sour.objectdrawingformat;
	origdataformat=sour.origdataformat;

	hirefll[0]=hirefll[1]=NULL;
	for (tp1=sour.hirefll[0]; tp1!=NULL; tp1=tp1->next)
	{ 
		tp2=new db_hireflink(*tp1); 
		addhiref(tp2); 
	}

	m_pResbufChain = NULL;
	m_OdClass  = NULL;
}

db_acad_proxy_object::~db_acad_proxy_object(void) 
{
	sds_relrb(m_pResbufChain);

	{
		try
		{
			OdDbObjectPtr pClass((OdDbObject*)m_OdClass, kOdRxObjAttach);
		}
		catch(...)
		{
			m_OdClass = NULL;
		}
	}

	db_hireflink::delll(hirefll[0]);
}

int db_acad_proxy_object::entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp)
{
/*
**	Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
	int rc;
	struct resbuf *sublist[2];
	int fint1;
	short fsh1;
	sds_name en;
	db_handitem *thip1;
	db_hireflink *refp1;

	rc=0; sublist[0]=sublist[1]=NULL;

	if (begpp==NULL || endpp==NULL) goto out;

	/* 90  Proxy object class ID(Always 499) */
	if ((sublist[0]=sublist[1]=db_newrb(90,'I',&pid))==NULL)
		{ rc=-1; goto out; }

	/* 91  Application object's class id */
	if ((sublist[1]->rbnext=db_newrb(91,'I',&aid))==NULL)
		{ rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;
	/* 95 Object drawing format code */
	// Note : objectdrawingformat is always zero with ODT 2.008
	if ((sublist[1]->rbnext=db_newrb(95,'I',&objectdrawingformat))==NULL)
		{ rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;

	/* 70 Original custom data format */
	// Note : origdataformat is always zero with ODT 2.008
	fsh1 = origdataformat;
	if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL)
		{ rc=-1; goto out; }
	sublist[1]=sublist[1]->rbnext;
	/* 330/340/350/360 Object Reference(s) */
	for( refp1 = hirefll[0] ; refp1 != NULL ; refp1 = refp1->next )
	{
		refp1->get_data(&fint1, NULL, &thip1, dp);
		switch( fint1 ){
		case DB_SOFT_POINTER: fint1 = 330; break;
		case DB_HARD_POINTER: fint1 = 340; break;
		case DB_SOFT_OWNER:   fint1 = 350; break;
		case DB_HARD_OWNER:   fint1 = 360; break;
		default:			  rc=-1; goto out;
		}
		en[0]=(long)thip1; en[1]=(long)dp;
		if ((sublist[1]->rbnext=db_newrb(fint1,'N',en))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;
	}

	/* 94  End marker of object id section */
	fint1 = 0;
	if ((sublist[1]->rbnext=db_newrb(94,'I',&fint1))==NULL)
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
// EBATECH(CNBR) ]-
