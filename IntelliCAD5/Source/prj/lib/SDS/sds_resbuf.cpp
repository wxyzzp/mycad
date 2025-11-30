/* D:\ICADDEV\PRJ\LIB\SDS\SDS_RESBUF.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"/*DNT*/
#include "icadlib.h"/*DNT*/
#include <stdarg.h>
#include <string.h>

/*DG - 6.7.2003*/// this turns on alloc/free resbuf by whole lists (using db_alloc_resbuf_list/db_free_resbuf_list),
// not one-by-one (db_alloc_resbuf/db_free_resbuf)
#define ALLOC_BY_LISTS

/****************************************************************************/
/**************** Linked List and Memory Functions **************************/
/****************************************************************************/

// *****************************************
// This is an SDS External API
//
// NOTE -- there is no sdsengine_ version of this
//
sds_resbuf* sds_buildlist(int nRType, ...)
{
/*
**  This function was designed to emulate the ADS function sds_buildlist()
**  INCLUDING IT'S QUIRKS.  Here are the unexpected quirks:
**
**      RTPOINT is treated exactly like RT3DPOINT: the Z-value IS taken,
**          so the source better be an sds_point -- not an array
**          of 2 sds_reals.
**      RTANG and RTORINT are stored in the list as RTREALs; that's what
**          they are, of course, but the ADS function CHANGES the restype
**          and won't put in RTANG or RTORINT.
**      All of the unpaired types (no value following) are given
**          a value of short -1: RTLB, RTLE, RTDOTE, RTVOID, RTNIL, RTT.
**      999 comment strings are not allowed for some reason; the
**          real sds_buildlist() returns NULL if one is included.
**          Perhaps they are for DXF files ONLY.
**
*/
#ifdef ALLOC_BY_LISTS

	va_list		ap;
	resbuf*		rblist = 0;
	size_t		n = 0;
	int			what2get;

	/*===============================================================
		The code "zero" should be transferred by a code "RTDXF0",
		instead of "0", as a "0" are corresponds end of list.
		It is an often mistake, therefore we here use macros "ASSERT"
	=================================================================*/
	ASSERT(nRType);
	ASSERT(nRType != RTNONE);

	// I. count list length
	va_start(ap, nRType);
	for(int rt = nRType; rt && rt != RTNONE; rt = va_arg(ap, int), ++n)
	{
		if(rt == 999)
		{
			va_end(ap);
			return 0;
		}
		if(rt == RTDXF0)
			rt = 0;

		if(rt == RTANG || rt == RTORINT)
			rt = RTREAL;

		what2get = ic_resval((short)rt);
		if(what2get == RTVOID || what2get == RTERROR)
		{
			va_end(ap);
			return 0;
		}

		switch(what2get)
		{
			case RTSTR:
				va_arg(ap, char*);
				break;
			case RTSHORT:
				va_arg(ap, short);
				break;
			case RTLONG:
				va_arg(ap, long);
				break;
			case RTREAL:
				va_arg(ap, sds_real);
				break;
			case RT3DPOINT:
				va_arg(ap, sds_real*);
				break;
			case RTENAME:
				va_arg(ap, long*);
				break;
			case RTBINARY:
				va_arg(ap, sds_binary*);
		}
	}

	if(!(rblist = db_alloc_resbuf_list(n)))
	{
		va_end(ap);
		return 0;
	}

	// II. fill the list with values
	bool		bSuccess = true;
	char*		str;
	sds_real*	pr;
	long*		pl;
	sds_binary*	pRbBinary;

	va_start(ap, nRType);
	for(resbuf* pRb = rblist; pRb && bSuccess; nRType = va_arg(ap, int), pRb = pRb->rbnext)
	{
		if(nRType == RTDXF0)
			nRType = 0;	// OK to use 0 now

		// Change rtype for those that the ADS sds_buildlist() changes
		// (see the quirks in the comments above):
		if(nRType == RTANG || nRType == RTORINT)
			nRType = RTREAL;

		// Here's where we capture rtype:
		pRb->restype = nRType;

		what2get = ic_resval((short)nRType);

		if(what2get == RTNONE)
			pRb->resval.rint = -1;	// The unpaired; see quirks comment

		// Get the values for the types that are paired with values:
		switch(what2get)
		{
			case RTSTR:
				str = va_arg(ap, char*);
				if(!str)
					break;
				if(pRb->resval.rstring = new char [strlen(str) + 1])
					strcpy(pRb->resval.rstring, str);
				else
					bSuccess = false;
				break;
			case RTSHORT:
				pRb->resval.rint = va_arg(ap, short);
				break;
			case RTLONG:
				pRb->resval.rlong = va_arg(ap, long);
				break;
			case RTREAL:
				pRb->resval.rreal = va_arg(ap, sds_real);
				break;
			case RT3DPOINT:
				pr = va_arg(ap, sds_real*);
				ic_ptcpy(pRb->resval.rpoint, pr);
				break;
			case RTENAME:
				pl = va_arg(ap, long*);
				ic_encpy(pRb->resval.rlname, pl);
				break;
			case RTBINARY:
				pRbBinary = va_arg(ap, sds_binary*);
				pRb->resval.rbinary.clen = pRbBinary->clen;
				if(pRb->resval.rbinary.buf = new char[pRbBinary->clen])
					memcpy(pRb->resval.rbinary.buf, pRbBinary->buf, pRbBinary->clen);
				else
					bSuccess = false;
		}
	}	// for

	va_end(ap);

	if(!bSuccess)
	{
		db_free_resbuf_list(rblist);
		return 0;
	}

#else // ALLOC_BY_LISTS

    char *fcp1;
    int what2get,rc;
	sds_real *rp;
	long *lp;
    struct resbuf *rblist,*prblist,*trbp1;
    struct sds_binary* rbbinary;
    va_list ap;

    rblist=prblist=trbp1=NULL;
	rc=0;

	/*===============================================================
		The code "zero" should be transferred by a code "RTDXF0",
		instead of "0", as a "0" are corresponds end of list.
		It is an often mistake, therefore we here use macros "ASSERT"
	=================================================================*/
	ASSERT(nRType!=0);
	ASSERT(nRType!=RTNONE);

    va_start(ap,nRType);
    for ( ; nRType!=0 && nRType!=RTNONE; nRType=va_arg(ap,int))
		{

        if (nRType==RTDXF0)
			nRType=0;  /* OK to use 0 now */

        if (nRType==999)
            {
			rc=-1;
			goto out;
			}  

	        if ((trbp1=db_alloc_resbuf())==NULL)
				{
				rc=-1;
				goto out;
				}

        /* Change rtype for those that the ADS sds_buildlist() changes */
        /* (see the quirks in the comments above): */
        if (nRType==RTANG || nRType==RTORINT)
			nRType=RTREAL;

        /* Here's where we capture rtype: */
        trbp1->restype=nRType;

        what2get=ic_resval((short)nRType);
        if (what2get==RTVOID || what2get==RTERROR) 
			{ 
			rc=-1;
			goto out;
			}
        if (what2get==RTNONE)
			trbp1->resval.rint=-1;  /* The unpaired; see quirks comment */

        /* Get the values for the types that are paired with values: */
        switch (what2get)
			{
            case RTSTR:
                fcp1=va_arg(ap,char *);
				if(fcp1==NULL) break;
                if ((trbp1->resval.rstring= new char [strlen(fcp1)+1])==NULL)
					{
					rc=-1;
					goto out;
					}
                strcpy(trbp1->resval.rstring,fcp1);
                break;
            case RTSHORT:
                trbp1->resval.rint=va_arg(ap,short);
                break;
            case RTLONG:
                trbp1->resval.rlong=va_arg(ap,long);
                break;
            case RTREAL:
                trbp1->resval.rreal=va_arg(ap,sds_real);
                break;
            case RT3DPOINT:
                rp=va_arg(ap,sds_real *);
                ic_ptcpy(trbp1->resval.rpoint,rp);
                break;
            case RTENAME:
                lp=va_arg(ap,long *);
                ic_encpy(trbp1->resval.rlname,lp);
                break;
            case RTBINARY:
                rbbinary = va_arg(ap,struct sds_binary*);
                trbp1->resval.rbinary.clen = rbbinary->clen;
                if ((trbp1->resval.rbinary.buf= new char [rbbinary->clen])==NULL)
					{
					rc=-1;
					goto out;
					}
                memcpy(trbp1->resval.rbinary.buf,rbbinary->buf,rbbinary->clen);
			}

        /* Link trbp1 into rblist: */
        if (rblist==NULL)
			rblist=trbp1;
		else
			prblist->rbnext=trbp1;
        prblist=trbp1;
        trbp1=NULL;  /* Important!  We just linked it into rblist, */
                     /* so don't let the "out:" free it. */
		}

out:
    va_end(ap);

    if (rc && rblist!=NULL) 
		{
		sds_relrb(rblist);
		rblist=prblist=NULL;
		}

    /* Free trbp1 if it was alloc'd but was never linked in: */
    if (trbp1!=NULL)
		{
		trbp1->rbnext=NULL;
		sds_relrb(trbp1);
		trbp1=NULL; 
		}

#endif // ALLOC_BY_LISTS

    return rblist;
}


// *****************************************
// This is an SDS External API
//
// Because of dependencies, this API has no sdsengine_ version
//
//
struct sds_resbuf *
sds_newrb(int nTypeOrDXF) 
	{

    struct resbuf *tp1;

    if (( tp1=db_alloc_resbuf() )==NULL)
		{
        return NULL;
		}

    tp1->restype=nTypeOrDXF;

    return tp1;
	}

// *****************************************
// This is an SDS External API
//
//
// Because of dependencies, this API has no sdsengine_ version
//
int 
sds_relrb(struct sds_resbuf *prbReleaseThis) 
{
/*
**  According to the documentation for ACAD's sds_relrb(), "If sds_relrb()
**  succeeds, it returns RTNORM; otherwise, it returns an error code."
**  It doesn't say what qualifies as an error or what the codes are.
**  As far as I can see, if the llist has an error in it, the freeing
**  will wake up the General: we can't tell a bogus pointer from a good
**  one.  Therefore, our version ALWAYS returns RTNORM.
**
**  Fixed in 12/97 to use ic_resval() instead of doing it's own
**  checks for strings and binary chunks.  (It didn't know about
**  102 strings.)  Also fixed it up to use my new freeing methods.
**
*/

#ifdef ALLOC_BY_LISTS

	for(resbuf* pRb = prbReleaseThis; pRb; pRb = pRb->rbnext)
	{
		switch(ic_resval(pRb->restype))
		{
		case RTSTR:
			IC_FREE(pRb->resval.rstring);
			break;
		case RTVOID:
		case RTBINARY:
			IC_FREE(pRb->resval.rbinary.buf);
		}
	}
	db_free_resbuf_list(prbReleaseThis);

#else

	int rv;
	struct resbuf *tp1;

	while (prbReleaseThis!=NULL) 
		{
		if ((rv=ic_resval(prbReleaseThis->restype))==RTSTR &&
			prbReleaseThis->resval.rstring!=NULL)
			{
                IC_FREE(prbReleaseThis->resval.rstring);
			}
		else if (rv==RTVOID && prbReleaseThis->resval.rbinary.buf!=NULL)
			{
            IC_FREE(prbReleaseThis->resval.rbinary.buf);
			}
		else if (rv==RTBINARY && prbReleaseThis->resval.rbinary.buf!=NULL)
			{
            IC_FREE(prbReleaseThis->resval.rbinary.buf);
			}
		tp1=prbReleaseThis->rbnext; 

		db_free_resbuf(prbReleaseThis);
		prbReleaseThis=tp1;
		}

#endif // ALLOC_BY_LISTS

	return RTNORM;
}
