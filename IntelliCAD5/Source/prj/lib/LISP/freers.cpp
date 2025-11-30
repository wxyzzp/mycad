/* C:\ICAD\PRJ\LIB\LISP\FREERS.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
#include "commandatom.h" /*DNT*/

static void lsp_freeExtraAtoms()
{
   // COMMENTED BY Sachin Dange for Bugzilla 78663
   // The variable "SDS_commandAtomObjPtrList" is not required now.
   //  So I removed "SDS_commandAtomObjPtrList" and "SDS_commandAtomList" variables from ICAD.

    /* void* key;
    class commandAtomObj* pAtom;
    for (POSITION pos = SDS_commandAtomObjPtrList->GetStartPosition(); pos != NULL; ) 
	{
        SDS_commandAtomObjPtrList->GetNextAssoc(pos, key, (void*&)pAtom);
        delete pAtom; 
    }
	SDS_commandAtomObjPtrList->RemoveAll();
    ASSERT(SDS_commandAtomObjPtrList->GetCount() == 0);
    ASSERT(SDS_commandAtomObjPtrList->IsEmpty());

    SDS_commandAtomList->RemoveAll();
    ASSERT(SDS_commandAtomList->GetCount() == 0);
    ASSERT(SDS_commandAtomList->IsEmpty());*/
}

/* ZONE: FREERS AND FUNCTIONS THAT MUST BE AWARE OF ALL LSP_RTtypes */
void lsp_freeatomsll(void) {
//  Frees the global atoms llists commandAtomObj.
//  It also frees the global llist of open file pointers and
//  the global llist of valid selection sets (closing and freeing them),
//  since all references to them are lost when the atoms llist
//  is destroyed.
    void *retkey;
    POSITION  Pos;
    class commandAtomObj *rValue;
    
    struct lsp_fileplink *tfp1,*tfp2;
    struct lsp_sslink    *tsp1,*tsp2;


	// Here we are using the commandAtomObjPtrList to remove the
	// list of commandAtomObj structurs.
    for (Pos = commandAtomObjPtrList->GetStartPosition(); Pos != NULL; ) {
        commandAtomObjPtrList->GetNextAssoc(Pos, retkey, (void*&)rValue);
        delete(rValue); 
    }
	// Now remove the key pointers too.
	commandAtomObjPtrList->RemoveAll();
    ASSERT(commandAtomObjPtrList->GetCount() == 0);
    ASSERT(commandAtomObjPtrList->IsEmpty());

	// commandAtomList can have two keys pointing to the same commandAtomObj.
	// For that reason we use the commandAtomObjPtrList to remove all the
	// commandAtomObj structures from memory above. Because commandAtomObjPtrList
	// only contains one key per structure. So here we only remove the key
	// pointers.
    commandAtomList->RemoveAll();
    ASSERT(commandAtomList->GetCount() == 0);
    ASSERT(commandAtomList->IsEmpty());
	lsp_freeExtraAtoms();
    commandAtomListStarted=0;

    // Free file pointers llist (and close them):
    for (tfp1=lsp_filepll; tfp1!=NULL; tfp1=tfp2) {
        if (tfp1->fp!=NULL) fclose(tfp1->fp);
        tfp2=tfp1->next; delete tfp1;
    }
    lsp_filepll=NULL;

    // Free selection sets llist (and ssfree them):
    for (tsp1=lsp_ssll; tsp1!=NULL; tsp1=tsp2)
        { sds_ssfree(tsp1->ss); tsp2=tsp1->next; delete tsp1; }
    lsp_ssll=NULL;

	g_lsp_bProtectedLisp = false;

}


void lsp_freeatoms(void) {
//  Frees the global atoms list commandAtomList which holds commandAtomObj(s).
//  It also frees the global llist of open file pointers and
//  the global llist of valid selection sets (closing and freeing them),
//  since all references to them are lost when the atoms llist
//  is destroyed.
	void *retkey;
    POSITION  Pos;
    commandAtomObj *rValue;
    struct lsp_fileplink *tfp1,*tfp2;
    struct lsp_sslink    *tsp1,*tsp2;

	// Here we are using the commandAtomObjPtrList to remove the
	// list of commandAtomObj structurs.
    for (Pos = commandAtomObjPtrList->GetStartPosition(); Pos != NULL; ) {
        commandAtomObjPtrList->GetNextAssoc(Pos, retkey, (void*&)rValue);
        delete(rValue);
    }
	// Now remove the key pointers too.
	commandAtomObjPtrList->RemoveAll();
    ASSERT(commandAtomObjPtrList->GetCount() == 0);
    ASSERT(commandAtomObjPtrList->IsEmpty());

	// commandAtomList can have two keys pointing to the same commandAtomObj.
	// For that reason we use the commandAtomObjPtrList to remove all the
	// commandAtomObj structures from memory above. Because commandAtomObjPtrList
	// only contains one key per structure. So here we only remove the key
	// pointers.
    commandAtomList->RemoveAll();
    ASSERT(commandAtomList->GetCount() == 0);
    ASSERT(commandAtomList->IsEmpty());
	lsp_freeExtraAtoms();
    commandAtomListStarted=0;

    // Free file pointers llist (and close them):
    for (tfp1=lsp_filepll; tfp1!=NULL; tfp1=tfp2) {
        if (tfp1->fp!=NULL) fclose(tfp1->fp);
        tfp2=tfp1->next; delete tfp1;
    }
    lsp_filepll=NULL;

    // Free selection sets llist (and ssfree them):
    for (tsp1=lsp_ssll; tsp1!=NULL; tsp1=tsp2)
        { sds_ssfree(tsp1->ss); tsp2=tsp1->next; delete tsp1; }
    lsp_ssll=NULL;

}

void lsp_freeatomlink(class commandAtomObj *ap) {
	ASSERT(ap != NULL);
	if (ap == NULL)
		return;

	commandAtomList->RemoveKey(ap->LName);
    if (ap->GName != NULL)
        commandAtomList->RemoveKey(ap->GName);

    commandAtomObjPtrList->RemoveKey(ap);
	delete(ap);
	ap = NULL;

	return;
}

void lsp_freesuprb(struct resbuf *rbp) {
/*
**  Frees a super resbuf (all links free-able -- unlike a special resbuf).
*/
    int fi1;
    struct resbuf *tp1;


    while (rbp!=NULL) {
// Ebatech(CNBR) Bugzilla#78486 2003/4/3 Entity list's STR restype is not RTSTR.
//#if 0 //<alm>
        if ((fi1=ic_resval(rbp->restype))==RTSTR || rbp->restype==LSP_RTSYMB) {
//#else
//        if ((fi1=rbp->restype)==RTSTR || fi1==LSP_RTSYMB) {
//#endif
            if (rbp->resval.rstring!=NULL) 
			{
				IC_FREE(rbp->resval.rstring);
				rbp->resval.rstring = NULL;
			}
        } else if (fi1==RTVOID && rbp->resval.rbinary.buf!=NULL)
		{
            IC_FREE(rbp->resval.rbinary.buf);
			rbp->resval.rbinary.buf = NULL;
		}
        tp1=rbp->rbnext;
		db_free_resbuf(rbp);
		rbp=tp1;
    }

}

void lsp_freesplrb(struct resbuf *rbp) {
/*
**  Frees SPECIAL super resbufs, in which the 1st link is defined (not
**  allocated) so it cannot be freed.  But the string and any other
**  allocated values may need freeing, and there MAY be a llist taking
**  off from the rbnext pointer that WAS allocated and needs freeing.
**
**  Sets the restype of the 1st link to RTNIL.
*/
    int link1, fi1;
    struct resbuf *tp1,*tp2;

    if (rbp==NULL) return;


    tp1=rbp; link1=1;
    while (tp1!=NULL) {
// Ebatech(CNBR) Bugzilla#78486 2003/4/3 Entity list's STR restype is not RTSTR.
//#if 0 //<alm>
        if ((fi1=ic_resval(tp1->restype))==RTSTR || tp1->restype==LSP_RTSYMB) {
//#else
//        if ((fi1=tp1->restype)==RTSTR || fi1==LSP_RTSYMB) {
//#endif
            if (tp1->resval.rstring!=NULL) 
			{
				IC_FREE(tp1->resval.rstring);
				tp1->resval.rstring = NULL;
			}
        } else if (fi1==RTVOID && tp1->resval.rbinary.buf!=NULL)
		{
            IC_FREE(tp1->resval.rbinary.buf);
			tp1->resval.rbinary.buf = NULL;
		}
        tp2=tp1->rbnext;
        if (link1)
			{
			link1=0;
			}
		else
			{
			db_free_resbuf(tp1);
			}
        tp1=tp2;
    }
    rbp->rbnext=NULL;
    rbp->restype=RTNIL;  /* SAFE DEFAULT -- AND OUR CODE COUNTS ON IT. */
    rbp->resval.rstring=NULL;  /* See lsp_argend() */

}

void lsp_freepend(void) {
    if (lsp_pend.expr!=NULL) IC_FREE(lsp_pend.expr);
    memset(&lsp_pend,0,sizeof(lsp_pend));
}

void lsp_freeiget(void) {
    if (lsp_iget.kwl!=NULL) { delete lsp_iget.kwl; lsp_iget.kwl=NULL; }
    lsp_iget.valid=0; lsp_iget.bits=0;
}

void lsp_freestrll(struct lsp_strlink *sll) {
    struct lsp_strlink *tp1;

    while (sll!=NULL) {
        tp1=sll->next; if (sll->str!=NULL) IC_FREE(sll->str);
        delete sll; sll=tp1;
    }
}


