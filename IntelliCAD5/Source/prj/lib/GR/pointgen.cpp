/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * functions to generate a point
 * 
 */ 

#include "gr.h"
#include "gr_view.h"
#include "pc2dispobj.h"
#include "DoBase.h"

extern gr_view gr_defview;

short gr_point2doll(
    sds_point                *entaxis,
    struct gr_displayobject **begpp,
    struct gr_displayobject **endpp,
    gr_view           *viewp) {
/*
**  An extension of gr_getdispobjs().
**  Allocates a display objects llist for the POINT currently being created
**  via gr_gensolid(). *begpp is pointed to the first link;
**  *endpp is pointed to the last link.
**
**  The strategy here is to build a unit-size "point" and then scale,
**  rotate, and translate it into place in it's NCS and
**  feed the point chains to gr_gensolid().  It's done in 3 stages:
**  base symbol; circle; square.
**
**  With POINT, we're doing a trick.  A POINT is generated as if it
**  were a planar entity -- it forms a symbol in the plane perpendicular
**  to its extrusion direction through the insertion point.  But its
**  10-point is WCS.  So, we transform it here to the ECS defined
**  by its extrusion (via entaxis) (and make sure that everywhere else
**  it is treated like a planar entity).
**
**  The 50-group rotation seems to be CW -- not CCW (because it's
**  an axis direction, not an entity rotation).  Applying it
**  as a rotation in the "fake ECS" discussed above seems to
**  generate it correctly.
**
**  Since linetype is always solid for POINTs (i.e., gr_doltype() isn't
**  called), it doesn't matter whether gr_ncs2ucs() is used now
**  or later.  I choose "later" by forcing viewp->ltsclwblk to 1
**  and then restoring its correct value in the "out".
**
**  Minimal error-checking is done here; that's up to gr_getdispobjs().
**
**  Returns: gr_getdispobjs() codes.
*/
    char *fcp1;
    short basetype,savltsclwblk,wpdmode,rc;
    long nintern;
    sds_real rot,cs,sn,ar1,ar2;
    sds_point pt,ipt;
    struct gr_displayobject *begdo,*enddo,*tdop1,*tdop2;
    struct gr_pclink *ilink;


    rc=0; begdo=enddo=NULL;

    //if (viewp==NULL || viewp->viewsize==0.0) viewp=&gr_defview;
	// View size CAN be 0.0 if only a point exists in a block.
	// (Brics find).
    if (viewp==NULL) viewp=&gr_defview;

    /* Trick gr_gensolid() into doing the gr_ncs2ucs() calls: */
    savltsclwblk=viewp->ltsclwblk; viewp->ltsclwblk=1;

    if (viewp->ment==NULL) { rc=-1; goto out; }

    fcp1=NULL;
    viewp->ment->get_8(&fcp1);
        wpdmode=(fcp1!=NULL && strisame("DEFPOINTS",fcp1)) ? 0 : viewp->pdmode;
    viewp->ment->get_10(pt);
    viewp->ment->get_50(&rot);

    cs=cos(rot); sn=sin(rot);

    /* pt is WCS, but we're treating POINT like a planar entity, */
    /* so transform it into the ECS specified by it's extrusion: */
    ipt[0]=pt[0]*entaxis[0][0]+pt[1]*entaxis[0][1]+pt[2]*entaxis[0][2];
    ipt[1]=pt[0]*entaxis[1][0]+pt[1]*entaxis[1][1]+pt[2]*entaxis[1][2];
    ipt[2]=pt[0]*entaxis[2][0]+pt[1]*entaxis[2][1]+pt[2]*entaxis[2][2];

    switch (basetype=(wpdmode&7)) {
        case 0:  /* Dot */
            if (viewp->allocatePointsChain(1)) 
			{ 
				rc = -3; 
				goto out; 
			}
            viewp->lupc=viewp->pc;
            viewp->lupc->swid=viewp->lupc->ewid=0.0;
            viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
            GR_PTCPY(viewp->lupc->pt,ipt);


			// Source Entity is the point
#ifdef _DOCLASSES_
			CDoBase::get().setEntity((db_entity *)viewp->ment);
			CDoBase::get().setViewData(viewp);
			CDoBase::get().setPointsData(viewp->pc, viewp->lupc, NULL, 0);
			CDoBase::get().setFlags(0, 0);
			CDoBase::get().setDotDir(NULL);
			if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
			if ((rc=gr_gensolid( (db_entity *)viewp->ment, viewp->pc,viewp->pc,NULL,
                0,0,0,NULL,&tdop1,&tdop2,viewp))!=0) 
#endif
				goto out;

            if (tdop1!=NULL && tdop2!=NULL) {
                if (begdo==NULL) 
					begdo=tdop1; 
				else if (NULL != enddo) 
					enddo->next=tdop1;
                enddo=tdop2;
            }
            break;
        case 2: case 3:  /* Plus, X */
            ar2=rot; if (basetype==3) ar2+=IC_PI/4.0;
            ar1=viewp->pdsize*cos(ar2);
            ar2=viewp->pdsize*sin(ar2);
            if (viewp->allocatePointsChain(2)) 
			{ 
				rc = -3; 
				goto out; 
			}
            viewp->lupc=viewp->pc;
            viewp->lupc->swid=viewp->lupc->ewid=0.0;
            viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
            viewp->lupc->pt[0]=ipt[0]-ar1;
            viewp->lupc->pt[1]=ipt[1]+ar2;
            viewp->lupc->pt[2]=ipt[2];
            viewp->lupc=viewp->lupc->next;
            viewp->lupc->swid=viewp->lupc->ewid=0.0;
            viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
            viewp->lupc->pt[0]=ipt[0]+ar1;
            viewp->lupc->pt[1]=ipt[1]-ar2;
            viewp->lupc->pt[2]=ipt[2];

#ifdef _DOCLASSES_
			CDoBase::get().setEntity((db_entity *)viewp->ment);
			CDoBase::get().setViewData(viewp);
			CDoBase::get().setPointsData(viewp->pc, viewp->lupc, NULL, 0);
			CDoBase::get().setFlags(0, 0);
			CDoBase::get().setDotDir(NULL);
			if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
            if ((rc=gr_gensolid( (db_entity *)viewp->ment, viewp->pc,viewp->lupc,NULL,
                0,0,0,NULL,&tdop1,&tdop2,viewp))!=0) 
#endif
				goto out;
            if (tdop1!=NULL && tdop2!=NULL) {
                if (begdo==NULL) 
					begdo=tdop1; 
				else if (NULL != enddo)
					enddo->next=tdop1;
                enddo=tdop2;
            }
            viewp->lupc=viewp->pc;
            viewp->lupc->swid=viewp->lupc->ewid=0.0;
            viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
            viewp->lupc->pt[0]=ipt[0]-ar2;
            viewp->lupc->pt[1]=ipt[1]-ar1;
            viewp->lupc->pt[2]=ipt[2];
            viewp->lupc=viewp->lupc->next;
            viewp->lupc->swid=viewp->lupc->ewid=0.0;
            viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
            viewp->lupc->pt[0]=ipt[0]+ar2;
            viewp->lupc->pt[1]=ipt[1]+ar1;
            viewp->lupc->pt[2]=ipt[2];

#ifdef _DOCLASSES_
			CDoBase::get().setEntity((db_entity *)viewp->ment);
			CDoBase::get().setViewData(viewp);
			CDoBase::get().setPointsData(viewp->pc, viewp->lupc, NULL, 0);
			CDoBase::get().setFlags(0, 0);
			CDoBase::get().setDotDir(NULL);
			if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
            if ((rc=gr_gensolid( (db_entity *)viewp->ment, viewp->pc,viewp->lupc,NULL,
                0,0,0,NULL,&tdop1,&tdop2,viewp))!=0) 
#endif
				goto out;
            if (tdop1!=NULL && tdop2!=NULL) {
                if (begdo==NULL) 
					begdo=tdop1; 
				else if (NULL != enddo)
					enddo->next=tdop1;
                enddo=tdop2;
            }
            break;
        case 4:  /* Tick mark */
            if (viewp->allocatePointsChain(2)) 
			{ 
				rc = -3; 
				goto out; 
			}
            viewp->lupc=viewp->pc;
            viewp->lupc->swid=viewp->lupc->ewid=0.0;
            viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
            GR_PTCPY(viewp->lupc->pt,ipt);
            viewp->lupc=viewp->lupc->next;
            viewp->lupc->swid=viewp->lupc->ewid=0.0;
            viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
            viewp->lupc->pt[0]=ipt[0]+0.5*viewp->pdsize*sn;
            viewp->lupc->pt[1]=ipt[1]+0.5*viewp->pdsize*cs;
            viewp->lupc->pt[2]=ipt[2];

#ifdef _DOCLASSES_
			CDoBase::get().setEntity((db_entity *)viewp->ment);
			CDoBase::get().setViewData(viewp);
			CDoBase::get().setPointsData(viewp->pc, viewp->lupc, NULL, 0);
			CDoBase::get().setFlags(0, 0);
			CDoBase::get().setDotDir(NULL);
			if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
            if ((rc=gr_gensolid( (db_entity *)viewp->ment, viewp->pc,viewp->lupc,NULL,
                0,0,0,NULL,&tdop1,&tdop2,viewp))!=0)
#endif
				goto out;
            if (tdop1!=NULL && tdop2!=NULL) {
                if (begdo==NULL) 
					begdo=tdop1; 
				else if (NULL != enddo)
					enddo->next=tdop1;
                enddo=tdop2;
            }
            break;
    }

    if (wpdmode&0x20) {  /* Add circle */
        if ((rc=gr_arc2pc(2,ipt[0],ipt[1],
            0.5*viewp->pdsize,0.0,0.0,ipt[2],0,0.0,0.0,
            viewp->ltsclwblk,
            fabs(viewp->GetPixelHeight())/viewp->absxsclp,
            viewp->curvdispqual,viewp))!=0) goto out;
        if ((nintern=viewp->upc-2L)<0L) nintern=0L;  /* # of internal pts */
        ilink=(nintern<1L) ? NULL : viewp->pc->next;  /* 1st internal pt */

#ifdef _DOCLASSES_
		CDoBase::get().setEntity((db_entity *)viewp->ment);
		CDoBase::get().setViewData(viewp);
		CDoBase::get().setPointsData(viewp->pc, viewp->lupc, ilink, nintern);
		CDoBase::get().setFlags(1, 0);
		CDoBase::get().setDotDir(NULL);
		if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
        if ((rc=gr_gensolid( (db_entity *)viewp->ment, viewp->pc,viewp->lupc,ilink,
            nintern,1,0,NULL,&tdop1,&tdop2,viewp))!=0) 
#endif
			goto out;
        if (tdop1!=NULL && tdop2!=NULL) {
            if (begdo==NULL) 
				begdo=tdop1; 
			else if (NULL != enddo)
				enddo->next=tdop1;
            enddo=tdop2;
        }
    }

    if (wpdmode&0x40) {  /* Add square */
        if (viewp->allocatePointsChain(5)) 
		{ 
			rc = -3; 
			goto out; 
		}
        viewp->lupc=viewp->pc;
        viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
        viewp->lupc->pt[0]=0.5*viewp->pdsize*(cs+sn);
        viewp->lupc->pt[1]=0.5*viewp->pdsize*(cs-sn);
        viewp->lupc->pt[2]=ipt[2];
        viewp->lupc=viewp->lupc->next;
        viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
        viewp->lupc->pt[0]=ipt[0]-viewp->pc->pt[1];
        viewp->lupc->pt[1]=ipt[1]+viewp->pc->pt[0];
        viewp->lupc->pt[2]=ipt[2];
        viewp->lupc=viewp->lupc->next;
        viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
        viewp->lupc->pt[0]=ipt[0]-viewp->pc->pt[0];
        viewp->lupc->pt[1]=ipt[1]-viewp->pc->pt[1];
        viewp->lupc->pt[2]=ipt[2];
        viewp->lupc=viewp->lupc->next;
        viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
        viewp->lupc->pt[0]=ipt[0]+viewp->pc->pt[1];
        viewp->lupc->pt[1]=ipt[1]-viewp->pc->pt[0];
        viewp->lupc->pt[2]=ipt[2];
        viewp->pc->pt[0]+=ipt[0];
        viewp->pc->pt[1]+=ipt[1];
        viewp->lupc=viewp->lupc->next;
        viewp->lupc->swid=viewp->lupc->ewid=0.0;
        viewp->lupc->startang=viewp->lupc->endang=IC_ANGLE_DONTCARE;
        GR_PTCPY(viewp->lupc->pt,viewp->pc->pt);

#ifdef _DOCLASSES_
		CDoBase::get().setEntity((db_entity *)viewp->ment);
		CDoBase::get().setViewData(viewp);
		CDoBase::get().setPointsData(viewp->pc, viewp->lupc, viewp->pc->next, 3);
		CDoBase::get().setFlags(1, 0);
		CDoBase::get().setDotDir(NULL);
		if((rc = CDoBase::get().getDisplayObjects(tdop1, tdop2)) != 0) 
#else
        if ((rc=gr_gensolid( (db_entity *)viewp->ment, viewp->pc,viewp->lupc,viewp->pc->next,
            3,1,0,NULL,&tdop1,&tdop2,viewp))!=0) 
#endif			
			goto out;
        if (tdop1!=NULL && tdop2!=NULL) {
            if (begdo==NULL) 
				begdo=tdop1; 
			else if (NULL != enddo)
				enddo->next=tdop1;
            enddo=tdop2;
        }
    }

out:
    if (rc) { gr_freedisplayobjectll(begdo); begdo=enddo=NULL; }
    *begpp=begdo; *endpp=enddo;

    viewp->ltsclwblk=savltsclwblk;

    return rc;
}

