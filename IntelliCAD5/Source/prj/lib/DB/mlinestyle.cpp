/* C:\ICAD\PRJ\LIB\DB\MLINESTYLE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"
#include "objects.h"

/***************************** db_mlinestyle *****************************/

db_mlinestyle::db_mlinestyle(void) : db_handitem(DB_MLINESTYLE) {
    deleted=0;
    name=desc=NULL; ang[0]=ang[1]=0.5*IC_PI; fillcolor=DB_BLCOLOR;
    flags=nelems=0; elem=NULL;
}
db_mlinestyle::db_mlinestyle(const db_mlinestyle &sour) :
    db_handitem(sour) {  /* Copy constructor */

    deleted=sour.deleted;

    name=NULL; db_astrncpy(&name,sour.name,-1);
    desc=NULL; db_astrncpy(&desc,sour.desc,-1);
    ang[0]   =sour.ang[0];
    ang[1]   =sour.ang[1];
    fillcolor=sour.fillcolor;
    flags    =sour.flags;
    nelems   =sour.nelems;
    elem=NULL;
    if (nelems>0 && sour.elem!=NULL) {
        elem=new db_mlselement[nelems];
        memcpy(elem,sour.elem,nelems*sizeof(elem[0]));
    }
}
db_mlinestyle::~db_mlinestyle(void) {
    delete [] name; delete [] desc; delete [] elem;
}


/*F*/
int db_mlinestyle::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) 
	{
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    int rc, i;
    struct resbuf *sublist[2];
	char *ltname;

    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

	//Get general info from db_handitem.
	db_handitem::entgetspecific(sublist, sublist + 1, dp);

	//Get the specific info from the db_mlinestyle
	for (i=0; i<nelems; i++)	
		{
		if ((sublist[1]->rbnext=db_newrb(49,'R',&(elem[i].offset)))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		if ((sublist[1]->rbnext=db_newrb(62,'I',&(elem[i].color)))==NULL) { rc=-1; goto out; }
		sublist[1]=sublist[1]->rbnext;

		elem[i].lthip->get_2(&ltname);
		if ((sublist[1]->rbnext=db_newrb(6,'S',(ltname!=NULL) ? ltname : db_str_quotequote))==NULL) { rc=-1; goto out; }
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
int db_mlinestyle::entmod(struct resbuf *modll, db_drawing *dp) 
	{
/*
**  Entity/table-record/object  specific.  Uses the setters for range-correction.
**
**  Returns: See db_handitem::entmod().
*/
    int rc, i, j, k;
    struct resbuf *curlink;
	double	*pOffset = NULL;
	int		*pColor = NULL;
	char	**pLTName = NULL;
    rc=0; db_lasterror=0;
    curlink=modll;

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
           curlink->restype ==0 ))
            curlink=curlink->rbnext;


    /* Walk; stop at the end or the EED sentinel (-3): Or stop when we see 71*/
    for (; !rc && curlink!=NULL && curlink->restype!=-3 && curlink->restype!=71; curlink=curlink->rbnext) 
		{
        /* Call the correct setter: */
        switch (curlink->restype) 
			{
            /*
            **  Some of the following strings are used to set table
            **  record references, requiring the set_n(char *, db_drawing *)
            **  form.  In those cases, we try BOTH calls, taking the
            **  one that returns true.
            */
            case   2: set_2(curlink->resval.rstring); break;
            case   3: set_3(curlink->resval.rstring); break;
            case  51: set_51(curlink->resval.rreal ); break;
            case  52: set_52(curlink->resval.rreal ); break;
            case  62: set_62(curlink->resval.rint  ); break;
            case  70: set_70(curlink->resval.rint  ); break;
            default:  entmodhi(&curlink,dp);          break;
                /* Catches persistent reactors and extension dictionary. */
			}

		}  /* End for each link */

	// EBATECH(CNBR) 2002/12/10 ICAD crashed if 71(nelem) is not in list.
	if( curlink == NULL || curlink->restype != 71 || curlink->resval.rint <= 0 )
		{ rc=-6; goto out; }

	nelems  = curlink->resval.rint;
	pColor	= new int	 [ nelems] ;
	pOffset	= new double [ nelems]; 
	pLTName	= new char * [ nelems]; 

	memset(pColor, 0,sizeof(int   )*nelems);
	memset(pOffset,0,sizeof(double)*nelems);
	memset(pLTName,0,sizeof(char *)*nelems);

	//Now continue walking the rest of the resbuf, collecting data into arrays representing each segment of the mlinstyle
    for (i=0,j=0,k=0; !rc && curlink!=NULL && curlink->restype!=-3; curlink=curlink->rbnext) 
		{
        switch (curlink->restype) 
			{
			case   6:
				ASSERT(i<nelems);
 				pLTName[i++] = curlink->resval.rstring;
				break;
			case  49:
				ASSERT(j<nelems);
 				pOffset[j++] = curlink->resval.rreal;
				break;
			case  62: 
				ASSERT(k<nelems);
				pColor[k++] = curlink->resval.rint;
				break;
			}
        }

	set_71 (nelems);
	set_6  (pLTName, nelems, dp);
	set_49 (pOffset, nelems);
	set_62 (pColor, nelems);

	if (db_lasterror) { rc=-6; goto out; }

	/* Do the EED: */
	if (!rc && curlink!=NULL) rc=set_eed(curlink,dp);

out:

	if (pColor) delete [] pColor;
	if (pOffset) delete [] pOffset;
	if (pLTName) delete [] pLTName;

    return rc;

	}


