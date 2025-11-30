/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Selector link functions
 * 
 */ 

#include "gr.h"

GR_API void gr_freeselector(struct gr_selectorlink *sl) {
    struct ic_pointsll *tp;

    if (sl==NULL) return;
    while (sl->def!=NULL) { tp=sl->def->next; delete sl->def; sl->def=tp; }
    if (sl->extent!=NULL) IC_FREE(sl->extent);
    delete sl;
}

GR_API struct gr_selectorlink *gr_initselector(
    char           type,
    char           mode,
    struct resbuf *rbdef) {
/*
**  Allocates a gr_selectorlink, sets it up, and returns a ptr
**  to it to the caller.
**
**  THE CALLER MUST FREE IT EVENTUALLY (with gr_freeselector()).
**
**  This function should be called to set up the gr_selectorlink
**  that gr_selected() will use.
**
**  "type" can be 'C'ircle, 'F'ence, poly'G'on, 'P'oint, or 'W'indow.
**
**  "mode"
**
**    "mode" can be 'I'nside, 'C' crossing, or 'O'utside.
**
**    For Point, it's "Is the selector point Inside or Outside of the
**    closed entity?".  (Crossing is interpreted as Inside.)
**
**    For all other types, the question is, "Is the entity Inside,
**    Crossing, or Outside the of selector?".  Fence can be Crossing or
**    Outside (meaning NOT crossing).  (Inside is interpreted as Crossing.)
**    The others are closed selectors: Inside means COMPLETELY inside;
**    Crossing means crossing OR completely inside; Outside means
**    COMPLETELY outside.
**
**  Here are the valid types and their rbdefs:
**
**     type           def (RP coord system)
**     ------------   -------------------------------------------
**     'C' (Circle)   RT3DPOINT (center), RTREAL (radius)
**     'F' (Fence)    2 or more RT3DPOINTs
**     'G' (polyGon)  3 or more RT3DPOINTs (closed implicitly)
**     'P' (Point)    1 RT3DPOINT
**     'W' (Window)   2 RT3DPOINTS (opposite corners, either pair,
**                      either order)
**
**  The struct gr_selectorlink uses an ic_pointsll llist for the
**  definition of the selector so that we can easily use ic_inorout()
**  for polyGon selectors.
**
**  Note that Window is preserved in gr_selectorlink, not converted
**  to a polyGon as I was initially tempted to do.  A window, with
**  its horizontal and vertical sides, makes possible certain
**  processing short cuts that a general polyGon can't use.
**
**  The selector must be specified in RP coordinates (to match the
**  display objects).
**
**  Returns a ptr to the allocated gr_selectorlink, or NULL if
**  something is wrong (check rc in the "out"):
**    -1 : Invalid type or mode.
**    -2 : Invalid rbdef for specified type.
**    -3 : No memory.
*/
    char *types,*modes;
    short rc;
    struct resbuf *trbp1;
    struct gr_selectorlink *sl;
    struct ic_pointsll *tplp1,*tplp2;


    rc=0; sl=NULL;
    types="CFGPW"; modes="ICO";

    type=toupper(type); mode=toupper(mode);

    /* Check type and mode: */
    if (strchr(types,type)==NULL || strchr(modes,mode)==NULL)
        { rc=-1; goto out; }

    /* Force correct mode for Fence and Point: */
    if (mode!='O') {
        if (type=='F') mode='C';
        if (type=='P') mode='I';
    }

    /* Check rbdef for each type: */
    if (rbdef==NULL ||
        (rbdef->rbnext==NULL && type!='P') ||
        (rbdef->rbnext==NULL && type=='G'))
            { rc=-2; goto out; }

    /* Alloc the struct: */
    if ((sl= new struct gr_selectorlink )==NULL) { rc=-3; goto out; }
	memset(sl,0,sizeof(struct gr_selectorlink));

    /* Set type and mode: */
    sl->type=type; sl->mode=mode;

    /* Define sl->def using rbdef: */

    if (type=='C') {  /* Circle (point, radius) */
        if ((rbdef->restype!=RT3DPOINT && rbdef->restype!=RTPOINT) ||
            rbdef->rbnext->restype!=RTREAL) { rc=-2; goto out; }

        if ((sl->def= new struct ic_pointsll )==NULL) { rc=-3; goto out; }
		memset(sl->def,0,sizeof(struct ic_pointsll));
        sl->def->pt[0]=rbdef->resval.rpoint[0];
        sl->def->pt[1]=rbdef->resval.rpoint[1];
        if ((sl->def->next= new struct ic_pointsll )==NULL) { rc=-3; goto out; }
		memset(sl->def->next,0,sizeof(struct ic_pointsll));
        sl->def->next->pt[0]=fabs(rbdef->rbnext->resval.rreal);
    } else {  /* All other types (chain of  pts) */
        tplp2=NULL;
        for (trbp1=rbdef; trbp1!=NULL; trbp1=trbp1->rbnext) {
            if (trbp1->restype!=RT3DPOINT && trbp1->restype!=RTPOINT)
                { rc=-2; goto out; }
            if ((tplp1= new struct ic_pointsll )==NULL) { rc=-3; goto out; }
			memset(tplp1,0,sizeof(struct ic_pointsll));
            tplp1->pt[0]=trbp1->resval.rpoint[0];
            tplp1->pt[1]=trbp1->resval.rpoint[1];
            if (sl->def==NULL) sl->def=tplp1; else tplp2->next=tplp1;
            tplp2=tplp1;
        }
    }

    /* Set sl->extent based on sl->def: */
    if ((sl->extent= new sds_point [2] )==NULL)
        { rc=-3; goto out; }
	memset(sl->extent,0,sizeof(sds_point)*2);
    if (sl->type=='C') {
        sl->extent[0][0]=sl->def->pt[0]-sl->def->next->pt[0];
        sl->extent[0][1]=sl->def->pt[1]-sl->def->next->pt[0];
        sl->extent[1][0]=sl->def->pt[0]+sl->def->next->pt[0];
        sl->extent[1][1]=sl->def->pt[1]+sl->def->next->pt[0];
    } else {  /* All other types (pt or chain of pts) */
        sl->extent[0][0]=sl->extent[1][0]=sl->def->pt[0];
        sl->extent[0][1]=sl->extent[1][1]=sl->def->pt[1];
        for (tplp1=sl->def->next; tplp1!=NULL; tplp1=tplp1->next) {
            if (sl->extent[0][0]>tplp1->pt[0])
                sl->extent[0][0]=tplp1->pt[0];
            else if (sl->extent[1][0]<tplp1->pt[0])
                     sl->extent[1][0]=tplp1->pt[0];
            if (sl->extent[0][1]>tplp1->pt[1])
                sl->extent[0][1]=tplp1->pt[1];
            else if (sl->extent[1][1]<tplp1->pt[1])
                     sl->extent[1][1]=tplp1->pt[1];
        }
    }


out:
    if (rc) {
        gr_freeselector(sl); sl=NULL;
    }
    return sl;
}


