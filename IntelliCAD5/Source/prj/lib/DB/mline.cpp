/* C:\ICAD\PRJ\LIB\DB\MLINE.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "db.h"

/**************************** db_mline *****************************/

db_mlelemlink::db_mlelemlink(void) {

    nlpars=napars=0; lpar=apar=NULL;
}
db_mlelemlink::~db_mlelemlink(void) {

    delete [] lpar; delete [] apar;
}

db_mlvertlink::db_mlvertlink(void) {

    pt[0]=pt[1]=pt[2]=0.0;
    dir[0]=dir[1]=dir[2]=0.0;
    miterdir[0]=miterdir[1]=miterdir[2]=0.0;
    elem=NULL;
}
db_mlvertlink::~db_mlvertlink(void) {

    delete [] elem;
}

db_mline::db_mline(void) : db_entity_with_extrusion(DB_MLINE) {

	mlship=NULL; start[0]=start[1]=start[2]=0.0;
    vert=NULL; scl=1.0; just=1; flags=1; nverts=nelems=0;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_mline::db_mline(db_drawing *dp) : db_entity_with_extrusion(DB_MLINE,dp) {

	mlship=NULL; start[0]=start[1]=start[2]=0.0;
    vert=NULL; scl=1.0; just=1; flags=1; nverts=nelems=0;

    disp=NULL; memset(extent,0,sizeof(extent));
}
db_mline::db_mline(const db_mline &sour) :
    db_entity_with_extrusion(sour) {  /* Copy constructor */

    int vidx,eidx;

    nverts=sour.nverts; nelems=sour.nelems;
    vert=NULL;
    if (nverts>0 && sour.vert!=NULL) {
        vert=new db_mlvertlink[nverts];
        for (vidx=0; vidx<nverts; vidx++) {
            DB_PTCPY(vert[vidx].pt      ,sour.vert[vidx].pt);
            DB_PTCPY(vert[vidx].dir     ,sour.vert[vidx].dir);
            DB_PTCPY(vert[vidx].miterdir,sour.vert[vidx].miterdir);
            vert[vidx].elem=NULL;
            if (nelems>0 && sour.vert[vidx].elem!=NULL) {
                vert[vidx].elem=new db_mlelemlink[nelems];
                for (eidx=0; eidx<nelems; eidx++) {
                         vert[vidx].elem[eidx].nlpars=
                    sour.vert[vidx].elem[eidx].nlpars;
                    vert[vidx].elem[eidx].lpar=NULL;
                    if (vert[vidx].elem[eidx].nlpars>0 &&
                        sour.vert[vidx].elem[eidx].lpar!=NULL) {

                        vert[vidx].elem[eidx].lpar=
                            new sds_real[vert[vidx].elem[eidx].nlpars];
                        memcpy(vert[vidx].elem[eidx].lpar,
                          sour.vert[vidx].elem[eidx].lpar,
                          vert[vidx].elem[eidx].nlpars*sizeof(sds_real));
                    }

                         vert[vidx].elem[eidx].napars=
                    sour.vert[vidx].elem[eidx].napars;
                    vert[vidx].elem[eidx].apar=NULL;
                    if (vert[vidx].elem[eidx].napars>0 &&
                        sour.vert[vidx].elem[eidx].apar!=NULL) {

                        vert[vidx].elem[eidx].apar=
                            new sds_real[vert[vidx].elem[eidx].napars];
                        memcpy(vert[vidx].elem[eidx].apar,
                          sour.vert[vidx].elem[eidx].apar,
                          vert[vidx].elem[eidx].napars*sizeof(sds_real));
                    }
                }
            }
        }
    }

    DB_PTCPY(start,sour.start);

    mlship=sour.mlship; scl=sour.scl; just=sour.just; flags=sour.flags;

    DB_PTCPY(extent[0],sour.extent[0]); DB_PTCPY(extent[1],sour.extent[1]);
    disp=NULL;  /* Don't copy display objects. */
}
db_mline::~db_mline(void) {

    delete [] vert; 

    if (db_grafreedofp!=NULL) db_grafreedofp(disp);
}

/*
**  When they were in the class def, these setters caused unresolved
**  external problems (destructors) because of the deletes in
**  them.  Moving them here solved the problem.  Note that they
**  are still inline.
*/
inline bool db_mline::set_72(int p) {
    delete [] vert; vert=NULL; nverts=0;
    if (p>1) {
        nverts=p; vert=new db_mlvertlink[nverts];
        memset(vert,0,nverts*sizeof(vert[0]));
    }
    return true;
}
inline bool db_mline::set_73(int p) {
    int fi1;
    if (p>0 && vert!=NULL) {
        nelems=p;
        for (fi1=0; fi1<nverts; fi1++) {
            delete [] vert[fi1].elem;
            vert[fi1].elem=new db_mlelemlink[nelems];
            memset(vert[fi1].elem,0,nelems*sizeof(vert[fi1].elem[0]));
        }
    }
    return true;
}
inline bool db_mline::set_74(int p, int vidx, int eidx) {
    if (vidx>-1 && vidx<nverts && eidx>-1 && eidx<nelems &&
        vert!=NULL && vert[vidx].elem!=NULL) {

        delete [] vert[vidx].elem[eidx].lpar;
        vert[vidx].elem[eidx].lpar=NULL;
        vert[vidx].elem[eidx].nlpars=0;
        if (p>0) {
            vert[vidx].elem[eidx].nlpars=p;
            vert[vidx].elem[eidx].lpar=new sds_real[p];
            memset(vert[vidx].elem[eidx].lpar,0,
                p*sizeof(vert[vidx].elem[eidx].lpar[0]));
        }
    }
    return true;
}
inline bool db_mline::set_75(int p, int vidx, int eidx) {
    if (vidx>-1 && vidx<nverts && eidx>-1 && eidx<nelems &&
        vert!=NULL && vert[vidx].elem!=NULL) {

        delete [] vert[vidx].elem[eidx].apar;
        vert[vidx].elem[eidx].apar=NULL;
        vert[vidx].elem[eidx].napars=0;
        if (p>0) {
            vert[vidx].elem[eidx].napars=p;
            vert[vidx].elem[eidx].apar=new sds_real[p];
            memset(vert[vidx].elem[eidx].apar,0,
                p*sizeof(vert[vidx].elem[eidx].apar[0]));

        }
    }
    return true;
}

/*F*/
int db_mline::entgetspecific(
    struct resbuf **begpp,
    struct resbuf **endpp,
    db_drawing     *dp) {
/*
**  Specific entgetter.  Returns 0 (OK) or -1 (no memory).
*/
    char *fcp1;
    short fsh1;
    int vidx,eidx,pidx,rc;
    sds_name en;
    struct resbuf *sublist[2];


    rc=0; sublist[0]=sublist[1]=NULL;

    if (begpp==NULL || endpp==NULL) goto out;

    /* 2 (mline style) */
    fcp1=NULL;
    if (mlship!=NULL) mlship->get_2(&fcp1);
    if ((sublist[0]=sublist[1]=db_newrb(2,'S',
        (fcp1!=NULL) ? fcp1 : db_str_quotequote))==NULL) { rc=-1; goto out; }

    /* 340 (mline style ename) */
    en[0]=(long)mlship; en[1]=(long)dp;
    if ((sublist[1]->rbnext=db_newrb(340,'N',en))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 40 (scl) */
    if ((sublist[1]->rbnext=db_newrb(40,'R',&scl))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 70 (just) */
    fsh1=(short)just;
    if ((sublist[1]->rbnext=db_newrb(70,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 71 (closure) */
    fsh1=(short)flags;
    if ((sublist[1]->rbnext=db_newrb(71,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 72 (nverts) */
    fsh1=(short)nverts;
    if ((sublist[1]->rbnext=db_newrb(72,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 73 (nelems) */
    fsh1=(short)nelems;
    if ((sublist[1]->rbnext=db_newrb(73,'H',&fsh1))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    /* 10 (start) */
    if ((sublist[1]->rbnext=db_newrb(10,'P',start))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

	// Extrusion is now part of the base class, so get it from accessor
	//
	sds_point extru;
	get_210( extru );

    /* 210 (extru) */
    if ((sublist[1]->rbnext=db_newrb(210,'P',extru))==NULL) { rc=-1; goto out; }
    sublist[1]=sublist[1]->rbnext;

    if (vert!=NULL) {
        for (vidx=0; vidx<nverts; vidx++) {

            /* 11 (pt) */
            if ((sublist[1]->rbnext=db_newrb(11,'P',vert[vidx].pt))==NULL) { rc=-1; goto out; }
            sublist[1]=sublist[1]->rbnext;

            /* 12 (dir) */
            if ((sublist[1]->rbnext=db_newrb(12,'P',vert[vidx].dir))==NULL) { rc=-1; goto out; }
            sublist[1]=sublist[1]->rbnext;

            /* 13 (miterdir) */
            if ((sublist[1]->rbnext=db_newrb(13,'P',vert[vidx].miterdir))==NULL) { rc=-1; goto out; }
            sublist[1]=sublist[1]->rbnext;

            if (vert[vidx].elem!=NULL) {
                for (eidx=0; eidx<nelems; eidx++) {

                    /* 74 (vert[vidx].elem[eidx].nlpars) */
                    fsh1=(short)vert[vidx].elem[eidx].nlpars;
                    if ((sublist[1]->rbnext=db_newrb(74,'H',&fsh1))==NULL) { rc=-1; goto out; }
                    sublist[1]=sublist[1]->rbnext;

                    if (vert[vidx].elem[eidx].lpar!=NULL) {
                        for (pidx=0; pidx<vert[vidx].elem[eidx].nlpars; pidx++) {

                            /* 41 (vert[vidx].elem[eidx].lpar[pidx]) */
                            if ((sublist[1]->rbnext=db_newrb(41,'R',
                                vert[vidx].elem[eidx].lpar+pidx))==NULL) { rc=-1; goto out; }
                            sublist[1]=sublist[1]->rbnext;

                        }
                    }

                    /* 75 (vert[vidx].elem[eidx].napars) */
                    fsh1=(short)vert[vidx].elem[eidx].napars;
                    if ((sublist[1]->rbnext=db_newrb(75,'H',&fsh1))==NULL) { rc=-1; goto out; }
                    sublist[1]=sublist[1]->rbnext;

                    if (vert[vidx].elem[eidx].apar!=NULL) {
                        for (pidx=0; pidx<vert[vidx].elem[eidx].napars; pidx++) {

                            /* 42 (vert[vidx].elem[eidx].apar[pidx]) */
                            if ((sublist[1]->rbnext=db_newrb(42,'R',
                                vert[vidx].elem[eidx].apar+pidx))==NULL) { rc=-1; goto out; }
                            sublist[1]=sublist[1]->rbnext;

                        }
                    }
                }
            }
        }
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
int db_mline::entmod(struct resbuf *modll, db_drawing *dp) {
/*
**  Entity/table-record specific.  Uses the setters for range-correction.
**
**  This one's hard.  It's a little inefficient: it does several
**  passes over modll, but the logic would be TOTALLY obscure if
**  I were to try and make it more efficient.  It's hard enough to follow
**  the way it is.
**
**  Returns: See db_handitem::entmod().
*/
    int nv,ne,vectcnt[3],n7x[2],vidx,ridx,its75,rarraysz,rc,fi1;
    sds_real  *rarray;
    sds_point *parray[3];
    struct resbuf *starthere,*curlink;
    db_handitem *thiship;


    rc=0; thiship=(db_handitem *)this;
    parray[0]=parray[1]=parray[2]=NULL; rarray=NULL;

    db_lasterror=0;

    if (modll==NULL || dp==NULL) { rc=-1; goto out; }

    del_xdict();

    starthere=modll;

    /*
    **  If modll has been built properly, the first few groups
    **  are things we don't allow to be modified (ename, type, handle),
    **  so let's walk over these right now:
    */
    while (starthere!=NULL &&
        (starthere->restype==-1 ||
         starthere->restype== 0 ||
         starthere->restype== 5)) starthere=starthere->rbnext;

    /*
    **  OK.  Do the tough part first: the nested arrays.
    **
    **  Count the verts, make sure each has 11-13 and at least one
    **  element (one 74 and one 75).  Make sure all vertices agree
    **  on the number of elements.
    **
    **  Here are the meanings of the counters:
    **
    **    For the whole MLINE:
    **
    **      nv : Number of vertices.
    **      ne : Number of elements.
    **
    **    For each vertex:
    **
    **      vectcnt[0] : Number of 11s.
    **      vectcnt[1] : Number of 12s.
    **      vectcnt[2] : Number of 13s.
    **          n7x[0] : Number of 74s.
    **          n7x[1] : Number of 75s.
    */

    /* First, just count stuff: */

    nv=ne=0;

    curlink=starthere;
    for (;;) {
        /* Step to the end, -3, or a 11-13: */
        while (curlink!=NULL && curlink->restype!=-3 &&
            (curlink->restype<11 || curlink->restype>13))
                curlink=curlink->rbnext;

        /* Finished? */
        if (curlink==NULL || curlink->restype==-3) break;

        /* Zero the counters for this vertex and walk to count the groups */
        /* until we reach the end, the -3, or until we reach a repeat */
        /* of a 11, 12, or 13: */
        for (vectcnt[0]=vectcnt[1]=vectcnt[2]=n7x[0]=n7x[1]=0; curlink!=NULL &&
            curlink->restype!=-3; curlink=curlink->rbnext) {

            switch (curlink->restype) {
                case 11: case 12: case 13: vectcnt[curlink->restype-11]++; break;
                case 74: case 75: n7x[curlink->restype-74]++; break;
            }

            /* Repeat of a 11, 12, or 13?: */
            if (vectcnt[0]>1 || vectcnt[1]>1 || vectcnt[2]>1) break;
        }

        /* Are we missing an 11, 12, or 13 for this vertex?  Do we have */
        /* NO elements?  Do the 74-count and 75-count disagree about */
        /* how many elements we have? */
        if (vectcnt[0]<1 || vectcnt[1]<1 || vectcnt[2]<1 ||
            n7x[0]<1 || n7x[0]!=n7x[1])
                { rc=-5; db_lasterror=OL_EMKMANDF; goto out; }

        /* If we haven't done so yet, capture the number of elements; */
        /* otherwise, make sure the latest vertex still agrees about */
        /* the number of elements: */
        if (ne<1) ne=n7x[0];
        else if (ne!=n7x[0]) { rc=-5; db_lasterror=OL_EMMFTYPE; goto out; }

        nv++;  /* Must be okay.  Increment the vertex counter. */
    }

    if (nv>1 && ne>0) {  /* Redefine the arrays. */

        set_72(nv); set_73(ne);

        /* Build the arrays of 11s, 12s, and 13s: */
        delete [] parray[0]; parray[0]=new sds_point[nv];
            memset(parray[0],0,nv*sizeof(parray[0][0]));
        delete [] parray[1]; parray[1]=new sds_point[nv];
            memset(parray[1],0,nv*sizeof(parray[1][0]));
        delete [] parray[2]; parray[2]=new sds_point[nv];
            memset(parray[2],0,nv*sizeof(parray[2][0]));
        vectcnt[0]=vectcnt[1]=vectcnt[2]=0;
        for (curlink=starthere; curlink!=NULL && curlink->restype!=-3; curlink=
            curlink->rbnext) {

            if ((fi1=curlink->restype-11)<0 || fi1>2 ||
                vectcnt[fi1]>=nv) continue;
            DB_PTCPY(parray[fi1][vectcnt[fi1]],curlink->resval.rpoint);
            vectcnt[fi1]++;
        }

        /* Set them: */
        set_11(parray[0]); set_12(parray[1]); set_13(parray[2]);

        /* Do the lpar[] and apar[] arrays for each vertex: */
        curlink=starthere; vidx=0;
        for (;;) {
            /* Step to the end, -3, or a 11-13: */
            while (curlink!=NULL && curlink->restype!=-3 &&
                (curlink->restype<11 || curlink->restype>13))
                    curlink=curlink->rbnext;

            /* Finished? */
            if (curlink==NULL || curlink->restype==-3) break;

            /* Zero the counters for this vertex and walk until we reach */
            /* the end, the -3, or until we reach a repeat of a 11, 12, or 13: */
            for (vectcnt[0]=vectcnt[1]=vectcnt[2]=n7x[0]=n7x[1]=0; curlink!=NULL &&
                curlink->restype!=-3; curlink=curlink->rbnext) {

                switch (curlink->restype) {
                    case 11: case 12: case 13:
                        vectcnt[curlink->restype-11]++;
                        break;
                    case 74: case 75:
                        its75=(curlink->restype==75);
                        /* How many 41s or 42s are supposed to follow? */
                        rarraysz=curlink->resval.rint;

                        /* Set n?pars and create ?par[]: */
                        if (its75) set_75(rarraysz,vidx,n7x[1]);
                        else       set_74(rarraysz,vidx,n7x[0]);

                        if (rarraysz>0) {
                            /* Create the necessary array of reals: */
                            delete [] rarray; rarray=new sds_real[rarraysz];
                            memset(rarray,0,rarraysz*sizeof(rarray[0]));

                            /* While we're still within rarray[] and */
                            /* there's another link and it's a 41 (or 42), */
                            /* take it and step to that link: */
                            for (ridx=0; ridx<rarraysz &&
                                curlink->rbnext!=NULL &&
                                curlink->rbnext->restype==41+its75; ridx++,
                                curlink=curlink->rbnext)
                                    rarray[ridx]=curlink->rbnext->resval.rreal;

                            /* Set the ?par[] array: */
                            if (its75) set_42(rarray,vidx,n7x[1]);
                            else       set_41(rarray,vidx,n7x[0]);
                        }
                        n7x[its75]++;
                        break;
                }

                /* Reached a repeat of a 11, 12, or 13?: */
                if (vectcnt[0]>1 || vectcnt[1]>1 || vectcnt[2]>1) break;
            }

            vidx++;  /* Increment the vertex idx. */
        }  /* End for (;;) (each vertex) */

    }  /* End if (nv>1 && ne>0) */


    /* Now do the simple ones: */

    /* Walk; stop at the end or the EED sentinel (-3): */
    for (curlink=starthere; !rc && curlink!=NULL &&
        curlink->restype!=-3; curlink=curlink->rbnext) {

        /* Call the correct setter: */
        switch (curlink->restype) {

            case  10: thiship->set_10(curlink->resval.rpoint); break;

            case  40: thiship->set_40(curlink->resval.rreal); break;

            case  70: thiship->set_70(curlink->resval.rint); break;
            case  71: thiship->set_71(curlink->resval.rint); break;

            case 210: thiship->set_210(curlink->resval.rpoint); break;

			case 340: thiship->set_340((db_handitem *)(curlink->resval.rlname[0])); break;

            default:  /* The common entity or table record groups. */
                rc=thiship->entmod1common(&curlink,dp);
                break;
        }
    }

    if (db_lasterror) { rc=-6; goto out; }

    /* Do EED: */
    if (!rc && curlink!=NULL) rc=set_eed(curlink,dp);

out:
    delete [] parray[0]; delete [] parray[1]; delete [] parray[2];
    delete [] rarray;

    return rc;
}



