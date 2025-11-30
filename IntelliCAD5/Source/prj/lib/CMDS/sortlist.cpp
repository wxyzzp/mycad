/* G:\ICADDEV\PRJ\LIB\CMDS\SORTLIST.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "LISP.H"		//ARCHANA

short cmd_sortlist(void) { 

	// Modified CyberAge AW 28/11/00 [
    // Reason: fix for Bug no : 46951
	// Check for valid datatypes for sorting strings
	struct resbuf *pList, *pListTemp = NULL;
	BOOL bFlagSymb = FALSE;
	BOOL bFlagInv	= FALSE;

	for(pList = lsp_argsll; pList != NULL; pList = pList->rbnext)
	{
		if(LSP_RTSYMB == pList->restype)	//check for symbols
		{
			if(stricmp(pList->resval.rstring, "t") == 0)	//"t/T" is a special case
			{
				bFlagInv = TRUE;
				continue;
			}
			bFlagSymb = TRUE;
			pListTemp = pList;
			break;
		}
		else if(RTSTR != pList->restype && RTLB != pList->restype && RTLE != pList->restype)
		{
			bFlagInv = TRUE;
		}
	}

	if(bFlagSymb)	//symbol in the list
	{
		if(NULL == pListTemp) return NULL;
		cmd_ErrorPrompt(CMD_ERR_BADARGUMENT, -1, "%s", pListTemp->resval.rstring, "\n");
		return lsp_prtrecur(pListTemp, NULL, 0,0);	
	}
	if(bFlagInv)	//any other invalid data type
	{
		return lsp_retnil();
	}
	// Modified CyberAge AW 28/11/00 ]

    struct resbuf *ab=sds_getargs(),*bb=NULL;
	bb=cmd_listsorter(ab);
	if(bb==NULL) sds_retval(NULL);
	else sds_retlist(bb);
	return(RTNORM);
}

struct resbuf *cmd_listsorter(struct resbuf *ab) { 
    int            type=0,
                   fi1,
                   fi2,
                   totcnt;
    struct resbuf *link,
                  *j,
                  *p,
                  *t,
                  *rb,
                  *rbb,gvrb;
    
	SDS_getvar(NULL,DB_QMAXSORT,&gvrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	fi1=0;
    // Make a copy of the link list (ab).
    for(t=rb=NULL,rbb=ab;rbb;rbb=rbb->rbnext) {
        if (!t) t=rb=sds_newrb(rbb->restype); else { rb->rbnext=sds_newrb(rbb->restype); rb=rb->rbnext; }
        switch(rb->restype) {
            case RTSTR:
                if(rbb->resval.rstring==NULL) break;
                //Bugzilla No. 78104 ; 15-04-2002 
				rb->resval.rstring= new char [strlen(rbb->resval.rstring)+1];
                strcpy(rb->resval.rstring,rbb->resval.rstring);
				++fi1;
                break;
            case RTSHORT:
                rb->resval.rint=rbb->resval.rint;
				++fi1;
                break;
            case RTREAL:
                rb->resval.rreal=rbb->resval.rreal;
				++fi1;
                break;
        }
    }
    // Set link to the beginning of the list.
    link=t;
    if(link==NULL) return(NULL);

	if(fi1>gvrb.resval.rint) return(link);  
    // Get rid of the RTLB and the RTLE from the copy of the link list.
    for(rb=link;rb;rbb=rb,rb=rb->rbnext) {
        if (rb->restype==RTLB) { 
            rbb=link; 
            link=rb=link->rbnext; 
            rbb->rbnext=NULL;
            sds_relrb(rbb);
        } else if (rb->restype==RTLE) {
            rbb->rbnext=NULL;
            sds_relrb(rb);
            rb=NULL;
            break;
        }
    }
    // Sort the link list.
    for(fi1=-1,totcnt=0;fi1<totcnt;++fi1) {
        // set the type of resbuf values.
        type=link->restype;
        for(fi2=0,p=j=link;j->rbnext;++fi2) {
            // If the restype is not the same throughout the list return error.
            if (j->restype!=type) return(NULL);
            switch(j->restype) {
                case RTSHORT:
                    if (j->rbnext->restype!=RTSHORT) { j=j->rbnext; break; }
                    if (j->resval.rint>j->rbnext->resval.rint) {
                        t=j->rbnext;
                        p->rbnext=j->rbnext;
                        j->rbnext=j->rbnext->rbnext;
                        t->rbnext=j;
                        if (j==link) p=link=t; else p=t;
                    } else { p=j; j=j->rbnext; }
                    break;
                case RTREAL:
                    if (j->rbnext->restype!=RTREAL) { j=j->rbnext; break; }
                    if (j->resval.rreal>j->rbnext->resval.rreal) {
                        t=j->rbnext;
                        p->rbnext=j->rbnext;
                        j->rbnext=j->rbnext->rbnext;
                        t->rbnext=j;
                        if (j==link) p=link=t; else p=t;
                    } else { p=j; j=j->rbnext; }
                    break;
                case RTSTR:
                    if (j->rbnext->restype!=RTSTR) { j=j->rbnext; break; }
                    if (strcmp(j->resval.rstring,j->rbnext->resval.rstring)>=0) {
                        t=j->rbnext;
                        p->rbnext=j->rbnext;
                        j->rbnext=j->rbnext->rbnext;
                        t->rbnext=j;
                        if (j==link) p=link=t; else p=t;
                    } else { p=j; j=j->rbnext; }
                    break;
                default:
                    j=j->rbnext;
                    break;
            }
            if ((totcnt && fi2>=totcnt-fi1) || j==NULL) break;
        }
        if (!totcnt) totcnt=fi2;
    }
    // Return the new copy of the link list.
    return(link);  
}



