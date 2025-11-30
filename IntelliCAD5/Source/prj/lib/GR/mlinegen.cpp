/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * special code to generate mlines
 * 
 */ 

#include "gr.h"
#include "gr_view.h"
#include "objects.h"
#include "linetypegen.h"
#include "pc2dispobj.h"
#include "DoBase.h"

int gr_mline2doll(
    sds_point         *entaxis,
    gr_displayobject **begdopp,
    gr_displayobject **enddopp,
    gr_view           *viewp,
	db_drawing* pDrawing,
	CDC* pDC) {
/*
**  An extension of gr_getdispobjs().
**  Allocates a display objects llist for MLINE. *begdopp is pointed
**  to the first link; *enddopp is pointed to the last link.
**
**  Minimal error-checking is done here; that's up to gr_getdispobjs().
**
**  NOTE:  This is one of those WCS entities that we treat like
**  an ECS entity (because we need to call the arc generator to
**  make the arc caps).
**
**  The geometry logic in this function (especially that for the arc caps)
**  counts on the following rules, which ACAD seems to maintain,
**  and which we'll have to follow as well:
**
**    Positive scale:
**
**      If you're walking the direction vector, the elements
**      occur in order from left to right, decreasing in offset
**      as they go.  That is, element [0] is the left-most
**      and has the greatest offset; element [sp_nelems-1] is the
**      right-most and has the smallest offset.  All positive
**      offsets are to the left of the center line defined
**      by the sp_vert[].pt's (11-groups); all negative offsets
**      are to the right.
**
**    Negative scale:
**
**      The opposite of the above (mirrored through the direction vector.
**
**  TODO
**  Things we still don't have:
**    Area fill
**
**  Returns: gr_getdispobjs() codes.
*/
    short savcolor;
    int v0,v1,lastv0,e0,didx,lpidx,ndos,*elemcolor,mlsflags,rc;
    int evennumlpars;
    int sp_nverts,sp_nelems,sp_flags;
    sds_real sp_scale;
    sds_point ecsvdir,ap1,ap2;
    gr_displayobject *doll[2],*tbegdo,*tenddo;
    db_handitem *savlthip,*sp_mlship,**elemlthip;
    db_mlvertlink *sp_vert;
	int fillcolor;
	sds_point fillpt1,fillpt2,fillpt3,fillpt4;

    sds_point **elemvert;  /* An chain of ECS vertices for each element: */
                           /*   elemvert[eidx][vidx]. */


	tbegdo=tenddo=NULL;
    rc=0; doll[0]=doll[1]=NULL; elemcolor=NULL; elemlthip=NULL; elemvert=NULL;sp_mlship=NULL;
    mlsflags=0;

    savcolor=viewp->color;
    savlthip=viewp->lthip;


    sp_vert=((db_mline *)(viewp->ment))->ret_vert();

    viewp->ment->get_40(&sp_scale);
    viewp->ment->get_71(&sp_flags);
    viewp->ment->get_72(&sp_nverts);
    viewp->ment->get_73(&sp_nelems);
    viewp->ment->get_340(&sp_mlship);
	ASSERT(sp_mlship);

	if (sp_mlship)	//in some cases (like copying a Mline from ACAD and pasting into ICAD) 340 is not defined. Bug 45919
		sp_mlship->get_62(&fillcolor);  // get fill color from style
										// note that there is another get62 which
										// gets the individual segments' colors
	else
		fillcolor = DB_BLCOLOR;

	if (fillcolor==DB_BBCOLOR)
		fillcolor=viewp->bbcolor;
	else if (fillcolor==DB_BLCOLOR)
		fillcolor=viewp->blcolor;
	
	if (sp_vert==NULL || sp_nverts<2 || sp_nelems<1) goto out;


    /* Calloc elemcolor, elemlthip, and elemvert. */
    /* (Note that we use separate arrays (instead of one array of structs) */
    /* because the get_6() and get_62() functions need them that way.) */
    if ((elemcolor= new int [sp_nelems] )==NULL ||
        (elemlthip= new db_handitem * [sp_nelems] )==NULL ||
        (elemvert=  new sds_point * [sp_nelems] )==NULL)
            { rc=-3; goto out; }

	memset(elemcolor, 0,sizeof(int)*sp_nelems);
	memset(elemlthip, 0,sizeof(db_handitem *)*sp_nelems);
	memset(elemvert,  0,sizeof(sds_point *)*sp_nelems);

    /* Set each element's default color and create each element's */
    /* array of vertices: */
    for (e0=0; e0<sp_nelems; e0++) {
        elemcolor[e0]=7;  /* Default color */

        /* elemlyhip[e0] is already NULL (calloc) */

        /* Create this element's vertex array: */
        if ((elemvert[e0]= new sds_point [ sp_nverts])==NULL) { rc=-3; goto out; }
		memset(elemvert[e0],0,sizeof(sds_point)*sp_nverts);

        /* Set this element's vertex by offsetting the true vertex */
        /* by lpar[0] in the miter direction -- and transform WCS to ECS: */
        for (v0=0; v0<sp_nverts; v0++) {
            /* Generate the WCS pt: */
            ap1[0]=sp_vert[v0].pt[0]+
                sp_vert[v0].elem[e0].lpar[0]*sp_vert[v0].miterdir[0];
            ap1[1]=sp_vert[v0].pt[1]+
                sp_vert[v0].elem[e0].lpar[0]*sp_vert[v0].miterdir[1];
            ap1[2]=sp_vert[v0].pt[2]+
                sp_vert[v0].elem[e0].lpar[0]*sp_vert[v0].miterdir[2];

            /* Convert WCS to ECS and put it into elemvert: */
            elemvert[e0][v0][0]=
                ap1[0]*entaxis[0][0]+ap1[1]*entaxis[0][1]+ap1[2]*entaxis[0][2];
            elemvert[e0][v0][1]=
                ap1[0]*entaxis[1][0]+ap1[1]*entaxis[1][1]+ap1[2]*entaxis[1][2];
            elemvert[e0][v0][2]=
                ap1[0]*entaxis[2][0]+ap1[1]*entaxis[2][1]+ap1[2]*entaxis[2][2];
        }
    }

    /* If the MLINESTYLE is set, get the elemcolor and elemlthip values */
    /* from it (and get the mlsflags): */
    if (sp_mlship!=NULL) {
        sp_mlship->get_70(&mlsflags);
        ((db_mlinestyle *)sp_mlship)->get_62(elemcolor,sp_nelems);
        ((db_mlinestyle *)sp_mlship)->get_6(elemlthip,sp_nelems);
        for (e0=0; e0<sp_nelems; e0++) {
            /* Set this element's resolved color: */
            if      (elemcolor[e0]==DB_BBCOLOR)
                elemcolor[e0]=viewp->bbcolor;
            else if (elemcolor[e0]==DB_BLCOLOR)
                elemcolor[e0]=viewp->blcolor;

            /* Set this element's resolved lthip: */
            if (elemlthip[e0]!=NULL) {
                if      (elemlthip[e0]->ret_byblock())
                    elemlthip[e0]=viewp->bblthip;
                else if (elemlthip[e0]->ret_bylayer())
                    elemlthip[e0]=viewp->bllthip;
            }
        }
    }

// MR 6/17/98 -- turn off caps if closed
	if (sp_flags & IC_MLINE_CLOSED)
		mlsflags &= ~(IC_MLSTYLE_START_ROUND | IC_MLSTYLE_END_ROUND | IC_MLSTYLE_START_INNER_ARC | IC_MLSTYLE_END_INNER_ARC);



// MR 6/17/98 -- make this a 2 pass process, draw the fills first, then the lines
	short pass;
	if (mlsflags & IC_MLSTYLE_FILL_ON) pass=0;
	else pass=1;
	
	for (; pass<2; pass++) {

		if (pass==0) viewp->ment->set_type(DB_SOLID);
		else viewp->ment->set_type(DB_MLINE);
		
		/* We're going to do 1 line seg (2 pts) per disp obj: */
	    if (pass==1 && viewp->allocatePointsChain(2)) { rc=-3; goto out; }
		else if (pass==0 && viewp->allocatePointsChain(5)) {rc=-3; goto out; }  // extra 1 to close
		viewp->lupc=viewp->pc->next;
	    /* No widths: */
	    viewp->pc->swid=viewp->pc->ewid=
			viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->pc->startang=viewp->pc->endang=
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;

		/* THE MAIN LINE-WORK (ELEMENTS) */

		/* Walk the segments (defined by v0 and v1) for open or closed: */
		for (v0=0,lastv0=sp_nverts-1-(!(sp_flags & IC_MLINE_CLOSED)); v0<=lastv0; v0++) {

			v1=(v0<lastv0 || !(sp_flags & IC_MLINE_CLOSED)) ? v0+1 : 0;  /* End vertex */

			/* Get the ECS direction vector (ecsvdir) by transforming */
			/* sp_vert[v0].dir WCS to ECS: */
			ecsvdir[0]=	sp_vert[v0].dir[0]*entaxis[0][0]+
						sp_vert[v0].dir[1]*entaxis[0][1]+
						sp_vert[v0].dir[2]*entaxis[0][2];
			ecsvdir[1]=	sp_vert[v0].dir[0]*entaxis[1][0]+
						sp_vert[v0].dir[1]*entaxis[1][1]+
						sp_vert[v0].dir[2]*entaxis[1][2];
			ecsvdir[2]=	sp_vert[v0].dir[0]*entaxis[2][0]+
						sp_vert[v0].dir[1]*entaxis[2][1]+
						sp_vert[v0].dir[2]*entaxis[2][2];

			for (e0=0; e0<sp_nelems; e0++) {	/* Each element */

			/* Get the # of disp objs for this element between these */
			/* vertices (v0 and v1): */
				ndos=sp_vert[v0].elem[e0].nlpars/2;
				evennumlpars=!(sp_vert[v0].elem[e0].nlpars%2);	/* An even number of */
														/* lpars? */
														/* (ACAD allows odd.) */
			/* Each disp object: */
				for (didx=0,lpidx=1; didx<ndos; didx++,lpidx+=2) {

		  /*
		  **  Get the start and stop pts.  Both pts are
		  **  offsets from elemvert[e0][v0] by the correct lpar[]
		  **  amount in the ecsvdir direction -- except for the
		  **  last end pt, which is elemvert[e0][v1].
		  */
		   
					sds_real *evptr=elemvert[e0][v0];
					ap1[0]=evptr[0]+sp_vert[v0].elem[e0].lpar[lpidx]*ecsvdir[0];
					ap1[1]=evptr[1]+sp_vert[v0].elem[e0].lpar[lpidx]*ecsvdir[1];
					ap1[2]=evptr[2]+sp_vert[v0].elem[e0].lpar[lpidx]*ecsvdir[2];
					if (didx>=ndos-1 && evennumlpars) {
						GR_PTCPY(ap2,elemvert[e0][v1]);
					} else {
						ap2[0]=evptr[0]+sp_vert[v0].elem[e0].lpar[lpidx+1]*ecsvdir[0];
						ap2[1]=evptr[1]+sp_vert[v0].elem[e0].lpar[lpidx+1]*ecsvdir[1];
						ap2[2]=evptr[2]+sp_vert[v0].elem[e0].lpar[lpidx+1]*ecsvdir[2];
					}

					if (viewp->ltsclwblk) {
						GR_PTCPY(viewp->pc->pt	,ap1);
						GR_PTCPY(viewp->lupc->pt,ap2);
					} else {
						gr_ncs2ucs(ap1,viewp->pc->pt  ,0,viewp);
						gr_ncs2ucs(ap2,viewp->lupc->pt,0,viewp);
					}

// capture fill points for fill at end
					if (didx==0) {
						if (e0==0) GR_PTCPY(fillpt1,viewp->pc->pt);
						if (e0==sp_nelems-1) GR_PTCPY(fillpt2,viewp->pc->pt);
					}
					if (didx==ndos-1) {
						if (e0==0) GR_PTCPY(fillpt3,viewp->lupc->pt);
						if (e0==sp_nelems-1) GR_PTCPY(fillpt4,viewp->lupc->pt);
					}

					if (pass==1) {// line pass
						viewp->pc->d2go=GR_DIST(viewp->pc->pt,viewp->lupc->pt);
						viewp->lupc->d2go=-1.0;
						viewp->color=elemcolor[e0];
						viewp->lthip=elemlthip[e0];
						if ((rc=gr_doltype( (db_entity*)viewp->ment, &tbegdo,&tenddo,viewp,pDrawing,pDC))!=0) goto out;
					}

					if (tbegdo!=NULL && tenddo!=NULL) {
						if (doll[0]==NULL) doll[0]=tbegdo; else doll[1]->next=tbegdo;
						doll[1]=tenddo;
					}
				}
			}  /* End for each element */


			if (pass==0) {
				viewp->color=fillcolor;
				short savltsclwblk=viewp->ltsclwblk;
		        viewp->ltsclwblk=0; // makes non-par ucs solids draw properly
				viewp->lupc=viewp->pc;
				GR_PTCPY(viewp->lupc->pt,fillpt2);
				viewp->lupc->swid=viewp->lupc->ewid=0.0;
		        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
				viewp->lupc=viewp->lupc->next;
				GR_PTCPY(viewp->lupc->pt,fillpt1);
				viewp->lupc->swid=viewp->lupc->ewid=0.0;
		        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
				viewp->lupc=viewp->lupc->next;
				GR_PTCPY(viewp->lupc->pt,fillpt3);
				viewp->lupc->swid=viewp->lupc->ewid=0.0;
		        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
				viewp->lupc=viewp->lupc->next;
				GR_PTCPY(viewp->lupc->pt,fillpt4);
				viewp->lupc->swid=viewp->lupc->ewid=0.0;
		        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
				viewp->lupc=viewp->lupc->next;
#ifdef _DOCLASSES_
				CDoBase::get().setEntity((db_entity *)viewp->ment);
				CDoBase::get().setViewData(viewp);
				CDoBase::get().setPointsData(viewp->pc, viewp->lupc, viewp->pc->next, 3);
				CDoBase::get().setFlags(1, 1);
				CDoBase::get().setDotDir(NULL);
				rc = CDoBase::get().getDisplayObjects(tbegdo, tenddo); 
#else
				rc=gr_gensolid( (db_entity *)viewp->ment, viewp->pc,viewp->lupc,viewp->pc->next,
				    3,1,1,NULL,&tbegdo,&tenddo,viewp);
#endif
				viewp->ltsclwblk=savltsclwblk; /* RESTORE CORRECT VAL. */
				if (rc!=0) goto out;
				if (tbegdo!=NULL && tenddo!=NULL) {
					if (doll[0]==NULL) doll[0]=tbegdo; else doll[1]->next=tbegdo;
					doll[1]=tenddo;
				}
			}

		}  /* End for each seg */


		/* Use the entity's ltype for all caps and miter lines: */
		viewp->lthip=savlthip;


				/* THE MITER LINES AND/OR LINE CAPS */

		int fi1;

if (pass==1)
	for (v0=0; v0<sp_nverts; v0++) {

		fi1=((sp_flags & IC_MLINE_CLOSED) && !(mlsflags & IC_MLSTYLE_SHOW_MITERS));	/* Closed and has no miter lines. */

		/* No beginning line if it's open and has no start line cap, */
		/* or it's closed and has no miter lines: */
		if (v0<1 && (!(sp_flags & IC_MLINE_CLOSED) && !(mlsflags & IC_MLSTYLE_START_SQUARE)) || fi1) continue;

		/* No ending one if it's open and has no ending line cap, */
		/* or it's closed and has no miter lines: */
		if (v0>=sp_nverts-1 && (!(sp_flags & IC_MLINE_CLOSED) && !(mlsflags & IC_MLSTYLE_END_SQUARE)) || fi1) continue;

		/* No internal ones if we don't have miter lines: */
		if (v0>0 && v0<sp_nverts-1 && !(mlsflags & IC_MLSTYLE_SHOW_MITERS)) continue;

		/* Get the midpt: */
		ap1[0]=0.5*(elemvert[0][v0][0]+elemvert[sp_nelems-1][v0][0]);
		ap1[1]=0.5*(elemvert[0][v0][1]+elemvert[sp_nelems-1][v0][1]);
		ap1[2]=0.5*(elemvert[0][v0][2]+elemvert[sp_nelems-1][v0][2]);

		/* 1st element to midpt: */
		if (viewp->ltsclwblk) {
			GR_PTCPY(viewp->pc->pt	,elemvert[0][v0]);
			GR_PTCPY(viewp->lupc->pt,ap1);
		} else {
			gr_ncs2ucs(elemvert[0][v0],viewp->pc->pt  ,0,viewp);
			gr_ncs2ucs(ap1			  ,viewp->lupc->pt,0,viewp);
		}
		viewp->pc->d2go=GR_DIST(viewp->pc->pt,viewp->lupc->pt);
		viewp->lupc->d2go=-1.0;
		viewp->color=elemcolor[0];
		if ((rc=gr_doltype( (db_entity*)viewp->ment, &tbegdo,&tenddo,viewp,pDrawing,pDC))!=0) goto out;
		if (tbegdo!=NULL && tenddo!=NULL) {
			if (doll[0]==NULL) doll[0]=tbegdo; else doll[1]->next=tbegdo;
			doll[1]=tenddo;
		}

		/* Midpt to last element: */
		GR_PTCPY(viewp->pc->pt,viewp->lupc->pt);
		if (viewp->ltsclwblk) {
			GR_PTCPY(viewp->lupc->pt,elemvert[sp_nelems-1][v0]);
		} else {
			gr_ncs2ucs(elemvert[sp_nelems-1][v0],viewp->lupc->pt,0,viewp);
		}

		viewp->pc->d2go=GR_DIST(viewp->pc->pt,viewp->lupc->pt);
		viewp->lupc->d2go=-1.0;
		viewp->color=elemcolor[sp_nelems-1];
		if ((rc=gr_doltype( (db_entity*)viewp->ment, &tbegdo,&tenddo,viewp,pDrawing,pDC))!=0) goto out;
		if (tbegdo!=NULL && tenddo!=NULL) {
			if (doll[0]==NULL) doll[0]=tbegdo; else doll[1]->next=tbegdo;
			doll[1]=tenddo;
		}
	}



	/* THE ARC CAPS */

	sds_real ar1,rad,ang[2];
	int hidx,cidx,e1,nacaps,ending,endingstart,endingend;
	sds_point cent;
			/* (See the geometry rules in the comments */
				 /* at the top of this function.) */

	if (sp_flags!=IC_MLINE_CLOSED) {	/* Only if it's open. */
		endingstart=0;
		endingend=2;
		if (sp_flags & IC_MLINE_SUPPRESS_START_CAPS) endingstart=1;
		if (sp_flags & IC_MLINE_SUPPRESS_END_CAPS) endingend=1;

		nacaps=sp_nelems/2;  /* The number of possible arc caps. */
		
		for (ending=endingstart; ending<endingend; ending++) {	/* Beginning arcs, then ending. */

			for (cidx=0; cidx<nacaps; cidx++) {  /* Each cap */

				/* Avoid outers if we aren't supposed to do them: */
				if (!cidx && !(mlsflags & ((ending) ? IC_MLSTYLE_END_ROUND : IC_MLSTYLE_START_ROUND))) continue;

				/* Avoid inners if we aren't supposed to do them: */
				if (cidx && !(mlsflags & ((ending) ? IC_MLSTYLE_END_INNER_ARC : IC_MLSTYLE_START_INNER_ARC))) break;

				/* We're pairing elements from the outside in.  Set e0 and e1 */
				/* such that the CCW arc will go from element e0 to e1. */
				/* (Note the reversal effect of a negative scale!) */
				if ((ending && sp_scale>=0.0) || (!ending && sp_scale<=0.0))
					{ e1=cidx; e0=sp_nelems-1-e1; }
				else
				{ e0=cidx; e1=sp_nelems-1-e0; }

				v0=(ending) ? sp_nverts-1 : 0;  /* Which vertex */

				/* Get the center (the midpoint of the line cap): */
				cent[0]=0.5*(elemvert[e0][v0][0]+elemvert[e1][v0][0]);
				cent[1]=0.5*(elemvert[e0][v0][1]+elemvert[e1][v0][1]);
				cent[2]=0.5*(elemvert[e0][v0][2]+elemvert[e1][v0][2]);

				rad=GR_DIST(elemvert[e0][v0],cent);

				for (hidx=0; hidx<2; hidx++) {  /* Each half */

				if (hidx) {  /* 2nd half */
					ar1=ang[0]+IC_PI; ang[0]=ang[1]; ang[1]=ar1;
				} else {  /* 1st half */
					/* The vector from e1 to e0 gives the start angle: */
					ap1[0]=elemvert[e0][v0][0]-elemvert[e1][v0][0];
					ap1[1]=elemvert[e0][v0][1]-elemvert[e1][v0][1];
					if (icadRealEqual(ap1[0],0.0) && icadRealEqual(ap1[1],0.0)) break;  /* Coincident element paths */
					ang[0]=atan2(ap1[1],ap1[0]);

					/* Use the ECS direction vector to get the end angle. */
					/* Use actual direction vectors -- not adjacent element vertices. */
					if (ending) {
					/* Xform the prev vertex's direction vector to the ECS */
					/* (just X and Y): */
						ap1[0]=	sp_vert[v0-1].dir[0]*entaxis[0][0]+
								sp_vert[v0-1].dir[1]*entaxis[0][1]+
								sp_vert[v0-1].dir[2]*entaxis[0][2];
						ap1[1]=	sp_vert[v0-1].dir[0]*entaxis[1][0]+
								sp_vert[v0-1].dir[1]*entaxis[1][1]+
								sp_vert[v0-1].dir[2]*entaxis[1][2];
					} else {
					/* Xform the NEGATIVE of this vertex's direction vector to the ECS */
					/* (just X and Y): */
						ap1[0]=	-sp_vert[v0].dir[0]*entaxis[0][0]-
								sp_vert[v0].dir[1]*entaxis[0][1]-
								sp_vert[v0].dir[2]*entaxis[0][2];
						ap1[1]=	-sp_vert[v0].dir[0]*entaxis[1][0]-
								sp_vert[v0].dir[1]*entaxis[1][1]-
								sp_vert[v0].dir[2]*entaxis[1][2];
					}
					if (icadRealEqual(ap1[0],0.0) && icadRealEqual(ap1[1],0.0)) break;  /* Coincident vertices? */
					ang[1]=atan2(ap1[1],ap1[0]);
				}

				/* Make the pc chain for this arc: */
				if (pass==1) {
					viewp->color=elemcolor[(hidx) ? e1 : e0];
				/* Ltype set earlier. */
					if ((rc=gr_arc2pc(0,cent[0],cent[1],rad,ang[0],ang[1],cent[2],0,
						0.0,0.0,viewp->ltsclwblk,fabs(viewp->GetPixelHeight())/viewp->absxsclp,
						viewp->curvdispqual,viewp))!=0) goto out;

					if ((rc=gr_doltype( (db_entity*)viewp->ment, &tbegdo,&tenddo,viewp,pDrawing,pDC))!=0) goto out;
				}
				
				else {  // pass==0 fill pass
					sds_real numchordsreal=0.3222*sqrt(viewp->curvdispqual*rad/(fabs(viewp->GetPixelHeight())/viewp->absxsclp))/4.0;
					sds_real angbase,angstep,ang1,lastcos,lastsin;
					int numchords=(int)numchordsreal;
					int i;

					viewp->color=fillcolor;
					angbase=ang[0];ang1=ang[1];
					ic_normang(&angbase,&ang1);
					angstep=(ang1-angbase)/numchords;
					fillpt2[2]=fillpt3[2]=cent[2];
					lastcos=cos(angbase);
					lastsin=sin(angbase);
					for (i=0; i<numchords; i++) {
						fillpt2[0]=cent[0]+rad*lastcos;
						fillpt2[1]=cent[1]+rad*lastsin;
						fillpt3[0]=cent[0]+rad*(lastcos=cos(angbase+angstep));
						fillpt3[1]=cent[1]+rad*(lastsin=sin(angbase+angstep));
						viewp->lupc=viewp->pc;
						GR_PTCPY(viewp->lupc->pt,cent);
						viewp->lupc->swid=viewp->lupc->ewid=0.0;
				        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
						viewp->lupc=viewp->lupc->next;
						GR_PTCPY(viewp->lupc->pt,fillpt2);
						viewp->lupc->swid=viewp->lupc->ewid=0.0;
				        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
						viewp->lupc=viewp->lupc->next;
						GR_PTCPY(viewp->lupc->pt,fillpt3);
						viewp->lupc->swid=viewp->lupc->ewid=0.0;
				        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
						viewp->lupc=viewp->lupc->next;
						short savltsclwblk=viewp->ltsclwblk;
						viewp->ltsclwblk=1; // makes non-par ucs solids draw properly

#ifdef _DOCLASSES_
						CDoBase::get().setEntity((db_entity *)viewp->ment);
						CDoBase::get().setViewData(viewp);
						CDoBase::get().setPointsData(viewp->pc, viewp->lupc, viewp->pc->next, 2);
						CDoBase::get().setFlags(1, 1);
						CDoBase::get().setDotDir(NULL);
						rc = CDoBase::get().getDisplayObjects(tbegdo, tenddo); 
#else
						rc=gr_gensolid( (db_entity *)viewp->ment, viewp->pc,viewp->lupc,viewp->pc->next,
					/* was 3 */		2,1,1,NULL,&tbegdo,&tenddo,viewp);
#endif
						viewp->ltsclwblk=savltsclwblk; /* RESTORE CORRECT VAL. */
						if (rc!=0) goto out;
						if (tbegdo!=NULL && tenddo!=NULL) {
							if (doll[0]==NULL) doll[0]=tbegdo; else doll[1]->next=tbegdo;
							doll[1]=tenddo;
						}
						angbase+=angstep;
					}
				}

				if (tbegdo!=NULL && tenddo!=NULL) {
					if (doll[0]==NULL) doll[0]=tbegdo; else doll[1]->next=tbegdo;
					doll[1]=tenddo;
				}

			}  /* End for each half */
	
		}  /* End for each cap on this end */

	  }  /* End for ending */

	}  /* End of arc caps */

	}	/* end of pass loop */


out:
    if (rc && doll[0]!=NULL) {
        gr_freedisplayobjectll(doll[0]);
        doll[0]=doll[1]=NULL;
    }

    if (begdopp!=NULL) *begdopp=doll[0];
    if (enddopp!=NULL) *enddopp=doll[1];
    if (elemcolor!=NULL) IC_FREE(elemcolor);
    if (elemlthip!=NULL) IC_FREE(elemlthip);
    if (elemvert!=NULL) {
        for (e0=0; e0<sp_nelems; e0++)
            if (elemvert[e0]!=NULL) IC_FREE(elemvert[e0]);
        IC_FREE(elemvert);
    }

    viewp->color=savcolor;
    viewp->lthip=savlthip;

    return rc;
}

