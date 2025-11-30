/* C:\ICAD\PRJ\LIB\LISP\DEBUG.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Published debug fns, also internal debug functions found throughout code during breakup
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/
#include "commandatom.h" /*DNT*/

int lsp_error(void) {
/*
**  For some reason, AutoLISP has an internal *ERROR* function that
**  seems to do nothing except return nil in all cases.
**  Well, here's ours.
*/

    lsp_retnil();

    return 0;
}

int lsp_trace(void)   { return lsp_tracem(1); }
int lsp_untrace(void) { return lsp_tracem(0); }

int lsp_tracem(int mode) {
    int msgn;
    struct resbuf *thisarg,*prevarg,*argsll;
    struct lsp_strlink *newlink,*before,*elink;

    msgn=-1; newlink=NULL;

    /* This is a donteval function (see lsp_lispev1()). */
    /* Steal lsp_argsll for local use (and don't free it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if (argsll==NULL) {

        #if defined(DEBUG)
            for (newlink=lsp_tracesyms; newlink!=NULL; newlink=newlink->next)
                sds_printf("\n|%s|"/*DNT*/,(newlink->str==NULL) ?
                    "NULL"/*DNT*/ : newlink->str);
        #endif

        goto out;
    }

    /* We don't need to use lsp_nextarg(), since we only allow symbols: */
    for (prevarg=NULL,thisarg=argsll; thisarg!=NULL; prevarg=thisarg,
        thisarg=thisarg->rbnext) {

        if (thisarg->restype!=LSP_RTSYMB ||
            thisarg->resval.rstring==NULL) { msgn=1; goto out; }

        /* See if we're already tracing it: */
        if ((elink=lsp_findstrlink(thisarg->resval.rstring,lsp_tracesyms,
            &before))!=NULL) {

            if (!mode) {  /* Remove it: */
                if (before==NULL) lsp_tracesyms=elink->next;
                else               before->next=elink->next;
                elink->next=NULL; lsp_freestrll(elink); elink=NULL;
            }

            /* If mode!=0 and we found it, continue. */

        } else if (mode) {

            /* Not found, and we're trying to add it. */

            /* Make the new link: */
            if ((newlink= new struct lsp_strlink )==NULL )
				{ msgn=7; goto out; }
			
			memset(newlink,0,sizeof(struct lsp_strlink));

            if ( (newlink->str = new char [strlen(thisarg->resval.rstring)+1]) ==NULL)
                    { msgn=7; goto out; }
			
            strcpy(newlink->str,thisarg->resval.rstring);

            /* Add it: */
            newlink->next=lsp_tracesyms; lsp_tracesyms=newlink;
            newlink=NULL;

        }
    }

    if (prevarg!=NULL && lsp_retval(prevarg)!=RTNORM) msgn=7;

out:
    if (newlink!=NULL) { lsp_freestrll(newlink); newlink=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

#if defined(DEBUG)

                /* ZONE: FUNCTIONS USED FOR DEBUGGING */


    void lsp_writeatoms(void) {
    /*
    **  For debugging.  Writes lsp_atomsll to ---debug.chk.
    */
        char typestr[81];
        class commandAtomObj *tap1;
        struct resbuf *trbp1;
        FILE *fptr;
        time_t tim;

        *typestr=0;

        if ((fptr=fopen("---debug.chk"/*DNT*/,"at"/*DNT*/))==NULL) return;

        time(&tim);
        fprintf(fptr,"\n\n****** START ************  %s\n"/*DNT*/,ctime(&tim));

        CString retkey;
        POSITION  Pos;
        for (Pos = commandAtomList->GetStartPosition(); Pos != NULL; ) {
            commandAtomList->GetNextAssoc(Pos, retkey, (CObject*&)tap1);

            if      (tap1->rb.restype==LSP_RTSUBR) strcpy(typestr,"LSP_RTSUBR"/*DNT*/);
            else if (tap1->rb.restype==LSP_RTXSUB) strcpy(typestr,"LSP_RTXSUB"/*DNT*/);
            else if (tap1->rb.restype==LSP_RTFILE) strcpy(typestr,"LSP_RTFILE"/*DNT*/);
            else if (tap1->rb.restype==LSP_RTSYMB) strcpy(typestr,"LSP_RTSYMB"/*DNT*/);
            else ic_adscode(tap1->rb.restype,typestr);

            fprintf(fptr,"\n type : %d (%s)\n"/*DNT*/,tap1->rb.restype,typestr);
            fprintf(fptr,  " name :|%s|\n"/*DNT*/,tap1->LName);
            fprintf(fptr,  "   id : %d\n"/*DNT*/,tap1->id);
            fprintf(fptr,  "value : "/*DNT*/);


            for (trbp1=&tap1->rb; trbp1!=NULL; trbp1=trbp1->rbnext) {

                if (trbp1!=&tap1->rb) {
                    typestr[0]=typestr[1]=' '/*DNT*/; typestr[2]=0;
                    if      (trbp1->restype==LSP_RTSUBR) strcpy(typestr+2,"LSP_RTSUBR"/*DNT*/);
                    else if (trbp1->restype==LSP_RTXSUB) strcpy(typestr+2,"LSP_RTXSUB"/*DNT*/);
                    else if (trbp1->restype==LSP_RTFILE) strcpy(typestr+2,"LSP_RTFILE"/*DNT*/);
                    else if (trbp1->restype==LSP_RTSYMB) strcpy(typestr+2,"LSP_RTSYMB"/*DNT*/);
                    else ic_adscode(trbp1->restype,typestr+2);
                    ic_rpad(typestr,13);
                    strcat (typestr,": "/*DNT*/);
                } else *typestr=0;

                switch (trbp1->restype) {
                    case RTREAL: case RTANG: case RTORINT:
                        fprintf(fptr,"%s%G\n"/*DNT*/,typestr,trbp1->resval.rreal);
                        break;
                    case RT3DPOINT: case RTPOINT:
                        fprintf(fptr,"%s(%G,%G,%G)\n"/*DNT*/,
                            typestr,trbp1->resval.rpoint[0],
                            trbp1->resval.rpoint[1],trbp1->resval.rpoint[2]);
                        break;
                    case RTSHORT:
                        fprintf(fptr,"%s%d\n"/*DNT*/,typestr,trbp1->resval.rint);
                        break;
                    case RTSTR: case LSP_RTSYMB:
                        fprintf(fptr,"%s|%s|\n"/*DNT*/,typestr,trbp1->resval.rstring);
                        break;
                    case RTENAME: case RTPICKS:
                        fprintf(fptr,"%s(0x%lX,0x%lX)\n"/*DNT*/,
                            typestr,trbp1->resval.rlname[0],trbp1->resval.rlname[1]);
                        break;
                    case RTLONG:
                        fprintf(fptr,"%s%ld\n"/*DNT*/,typestr,trbp1->resval.rlong);
                        break;
                    case LSP_RTSUBR: case LSP_RTXSUB: case LSP_RTFILE:
                        fprintf(fptr,"%s%p\n"/*DNT*/,typestr,trbp1->resval.rstring);
                        break;
                    default:
                        fprintf(fptr,"%sNONE\n"/*DNT*/,typestr);
                        break;
                }
            }
        }

        time(&tim);
        fprintf(fptr,"\n\n******** END ************  %s\n"/*DNT*/,ctime(&tim));
        fclose(fptr); fptr=NULL;
    }

    void lsp_writesuprb(struct resbuf *suprb) {
    /*
    **  For debugging.  Writes llist suprb to ---debug.chk.
    */
        char fs1[IC_ACADBUF],fc1;
        int cd,mod100,itsdxf,fi1;
        struct resbuf *tp1;
        FILE *fptr;
        time_t tim;

        if ((fptr=fopen("---debug.chk"/*DNT*/,"at"/*DNT*/))==NULL) return;
        time(&tim);
        fprintf(fptr,"\n\n****** START ************  %s\n"/*DNT*/,ctime(&tim));

        for (tp1=suprb; tp1!=NULL; tp1=tp1->rbnext) {

            cd=tp1->restype; mod100=cd%100;
            itsdxf=(cd>-5 && cd<2000);

            if (itsdxf) {
                sprintf(fs1,"%d"/*DNT*/,tp1->restype);
            } else {
                if      (cd==LSP_RTSUBR) strcpy(fs1,"LSP_RTSUBR"/*DNT*/);
                else if (cd==LSP_RTXSUB) strcpy(fs1,"LSP_RTXSUB"/*DNT*/);
                else if (cd==LSP_RTSYMB) strcpy(fs1,"LSP_RTSYMB"/*DNT*/);
                else if (cd==LSP_RTFILE) strcpy(fs1,"LSP_RTFILE"/*DNT*/);
                else ic_adscode(cd,fs1);
            }
            fprintf(fptr,"\n%12s : "/*DNT*/,fs1);

            if ((itsdxf && mod100>38 && mod100<60) || cd==RTREAL || cd==RTANG ||
                cd==RTORINT) {

                fprintf(fptr,"%G"/*DNT*/,tp1->resval.rreal);
            } else if ((itsdxf && mod100>9 && mod100<39) || cd==RTPOINT || cd==RT3DPOINT) {
                fprintf(fptr,"(%G,%G,%G)"/*DNT*/,
                    tp1->resval.rpoint[0],
                    tp1->resval.rpoint[1],
                    tp1->resval.rpoint[2]);
            } else if ((itsdxf && mod100>59 && mod100<80) || cd==RTSHORT) {
                fprintf(fptr,"%d"/*DNT*/,tp1->resval.rint);
            } else if ((itsdxf && mod100>-1 && mod100<10) || cd==-4 ||
                cd==RTSTR || cd==LSP_RTSYMB) {

                if (cd==1004) {
                    for (fi1=0; fi1<tp1->resval.rbinary.clen &&
                        fi1<127; fi1++) {

                        fprintf(fptr,"%d"/*DNT*/,tp1->resval.rbinary.buf[fi1]);
                        if (fi1<tp1->resval.rbinary.clen-1 && fi1<126)
                            fputc(','/*DNT*/,fptr);
                    }
                } else {
                    if (tp1->resval.rstring==NULL) {
                        fprintf(fptr,"NULL"/*DNT*/);
                    } else {
                        fc1=0;
                        if (strlen(tp1->resval.rstring)>100) {
                            fc1=tp1->resval.rstring[100];
                            tp1->resval.rstring[100]=0;
                        }
                        ic_escconv(tp1->resval.rstring,fs1,'\\'/*DNT*/,lsp_true2esc,
                            1,1);
                        if (fc1) tp1->resval.rstring[100]=fc1;
                        fprintf(fptr,"|%0.120s|"/*DNT*/,fs1);
                    }
                }
            } else if (cd==-2 || cd==-1 || cd==RTENAME || cd==RTPICKS) {
                fprintf(fptr,"(0x%lX,0x%lX)"/*DNT*/,
                    tp1->resval.rlname[0],
                    tp1->resval.rlname[1]);
            } else if (cd==1071 || cd==RTLONG) {
                fprintf(fptr,"%ld"/*DNT*/,tp1->resval.rlong);
            } else if (cd==LSP_RTSUBR || cd==LSP_RTXSUB || cd==LSP_RTFILE) {
                fprintf(fptr,"%p"/*DNT*/,tp1->resval.rstring);
            }

        }

        time(&tim);
        fprintf(fptr,"\n\n******** END ************  %s\n"/*DNT*/,ctime(&tim));
        fclose(fptr); fptr=NULL;
    }

    void lsp_checkatoms(void) {
        char typestr[41],*name;
        int type,aidx = 0;
        struct resbuf *trbp1;
        class commandAtomObj *tap1,*tap2;

        CString retkey;
        POSITION  Pos;
        for (Pos = commandAtomList->GetStartPosition(); Pos != NULL; ) {
            commandAtomList->GetNextAssoc(Pos, retkey, (CObject*&)tap1);

        type=tap1->rb.restype;
        if      (type==LSP_RTSUBR) strcpy(typestr,"LSP_RTSUBR"/*DNT*/);
        else if (type==LSP_RTXSUB) strcpy(typestr,"LSP_RTXSUB"/*DNT*/);
        else if (type==LSP_RTSYMB) strcpy(typestr,"LSP_RTSYMB"/*DNT*/);
        else if (type==LSP_RTFILE) strcpy(typestr,"LSP_RTFILE"/*DNT*/);
        else ic_adscode(type,typestr);

        if (tap1->LName==NULL) {
            sds_printf("\nAtom name is NULL at %d."/*DNT*/,aidx);
            name="*NULL*"/*DNT*/;
        } else name=tap1->LName;

        if (!*name) sds_printf("\nAtom name is \"\" at %d."/*DNT*/,aidx);

        /* Check for bad type: */
        if (type!=RTSHORT    && type!=RTLONG     && type!=RTREAL     &&
            type!=RTANG      && type!=RTORINT    && type!=RTSTR      &&
            type!=RTENAME    && type!=RTPICKS    && type!=RTT        &&
            type!=RTNIL      && type!=RTLB       && type!=LSP_RTSYMB &&
            type!=LSP_RTSUBR && type!=LSP_RTXSUB && type!=LSP_RTFILE) {

            sds_printf("\nInvalid type (%s/%d) for %s at %d"/*DNT*/,
                typestr,type,name,aidx);
        }

        /* Check for NULL rstring vals: */
        if ((type==RTSTR || type==LSP_RTSYMB || type==LSP_RTSUBR ||
            type==LSP_RTXSUB || type==LSP_RTFILE) &&
            tap1->rb.resval.rstring==NULL) {

            sds_printf("\n%s (%s) with NULL value at %d."/*DNT*/,
                name,typestr,aidx);
        }

        /* Check for possible garbage in reals: */
        if ((type==RTREAL || type==RTANG || type==RTORINT) &&
            (tap1->rb.resval.rreal<-1.0E+20 ||
             tap1->rb.resval.rreal> 1.0E+20 ||
            (!icadRealEqual(tap1->rb.resval.rreal,0.0) &&
             tap1->rb.resval.rreal>-1.0E-20 &&
             tap1->rb.resval.rreal< 1.0E-20))) {

            sds_printf("\n%s (%s) with unusual value %G at %d"/*DNT*/,
                name,typestr,tap1->rb.resval.rreal,aidx);
        }

        /* Check for zeroed rlnames: */
        if ((type==RTENAME || type==RTPICKS) &&
            !tap1->rb.resval.rlname[0] && !tap1->rb.resval.rlname[1]) {

            sds_printf("\n%s (%s) with value (0L,0L) at %d"/*DNT*/,
                name,typestr,aidx);
        }

        /* Check for unterminated lists and values that extend beyond */
        /* one value: */
        if ((trbp1=lsp_argend(&tap1->rb,NULL))==NULL)
            sds_printf("\n%s (%s) is an unterminated list at %d."/*DNT*/,
                name,typestr,aidx);
        else if (trbp1->rbnext!=NULL)
            sds_printf("\nMore data in llist beyond value for %s (%s) at %d."/*DNT*/,
                name,typestr,aidx);

        /* Check if it's a dup atom name: */
        if (tap1->LName!=NULL) {

                commandAtomList->Lookup((LPCTSTR)tap1->LName, (CObject*&)tap2);

                if (tap1==tap2 || tap2->LName==NULL)
                    continue;

                if (strisame(tap1->LName,tap2->LName))
                    break;

                if (tap2!=NULL)
                    sds_printf("\nDuplicate atom name (%s) at %d."/*DNT*/,name,aidx);
            }
        }
    }


    int lsp_test(void) {
        char fs1[81],*fcp1,*fcp2;
        unsigned char *buf,fuc1;
        int bit,npf,fi1;
        long byt,bufsz,pf[32],n1,n2;
        FILE *fptr;

        buf=NULL; fptr=NULL;

        if (sds_getstring(0,"\nEnter largest # to flag: "/*DNT*/,fs1)!=
            RTNORM) goto out;

        bufsz=ic_mkprimes(atol(fs1),&buf);
        sds_printf("\nbufsz: %ld"/*DNT*/,bufsz);
        if (bufsz<1L) goto out;

        if ((fptr=fopen("t.t"/*DNT*/,"wb"/*DNT*/))==NULL) goto out;
        fwrite(buf,sizeof(char),bufsz,fptr);
        fclose(fptr); fptr=NULL;

        if ((fptr=fopen("tt.t"/*DNT*/,"wt"/*DNT*/))==NULL) goto out;
        fi1=0;
        for (byt=0L; byt<bufsz; byt++) {
            fuc1=buf[byt];
            for (bit=0; bit<8; bit++,fuc1>>=1) {
                if (fuc1&0x01) {
                    fprintf(fptr,"%6ld"/*DNT*/,byt*8L+bit);
                    if (++fi1>11) { fputc('\n'/*DNT*/,fptr); fi1=0; }
                }
            }
        }
        if (fi1) fputc('\n'/*DNT*/,fptr);
        fclose(fptr); fptr=NULL;

        for (;;) {
            if (sds_getstring(0,"\nEnter N1,N2 (#'s or products): "/*DNT*/,fs1)!=
                RTNORM) break;

            if ((fcp1=strchr(fs1,','/*DNT*/))==NULL) continue;

            *fcp1=0;
            fcp2=fs1; n1=atol(fcp2);
            while ((fcp2=strchr(fcp2,'*'/*DNT*/))!=NULL)
                { fcp2++; n1*=(long)atof(fcp2); }
            *fcp1=','/*DNT*/;

            fcp2=fcp1+1; n2=atol(fcp2);
            while ((fcp2=strchr(fcp2,'*'/*DNT*/))!=NULL)
                { fcp2++; n2*=(long)atof(fcp2); }

            npf=ic_primefac(n1,buf,bufsz,pf);
            sds_printf("\n%2d:"/*DNT*/,npf);
            for (fi1=0; fi1<32 && fi1<npf; fi1++)
                sds_printf(" %ld"/*DNT*/,pf[fi1]);

            npf=ic_primefac(n2,buf,bufsz,pf);
            sds_printf("\n%2d:"/*DNT*/,npf);
            for (fi1=0; fi1<32 && fi1<npf; fi1++)
                sds_printf(" %ld"/*DNT*/,pf[fi1]);

            sds_printf("\n  GCD: %ld"/*DNT*/,ic_gcd(n1,n2,buf,bufsz));
        }

    out:
        if (buf!=NULL) { IC_FREE(buf); buf=NULL; }
        if (fptr!=NULL) { fclose(fptr); fptr=NULL; }
        return 0;
    }

    int lsp_chkatoms(void) {
        lsp_checkatoms();
        return 0;
    }

    int lsp_chkss(void) {
        struct lsp_sslink *tsp1;

        for (tsp1=lsp_ssll; tsp1!=NULL; tsp1=tsp1->next)
            sds_printf("\n0x%lX,0x%lX"/*DNT*/,tsp1->ss[0],tsp1->ss[1]);
        return 0;
    }


#endif



