/* C:\ICAD\PRJ\LIB\LISP\APP.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

// TODO all of these functions are unfinished and need work!!!!!!!!
int lsp_autoarxload(void) {   lsp_retnil();  return(0); }
int lsp_autoload(void) {      lsp_retnil();  return(0); }
int lsp_autoxload(void) {     lsp_retnil();  return(0); }

int lsp_startapp(void) {
	char fs1[IC_ACADBUF];
	int pc;
    int msgn;
    struct resbuf *tp1;

    msgn=-1;

	fs1[0]=0;
    for (tp1=lsp_argsll; tp1!=NULL; tp1=tp1->rbnext) {
        if (tp1->restype==RTSTR) {
			if(*fs1) strncat(fs1," "/*DNT*/,sizeof(fs1)-strlen(fs1)-1);
            strncat(fs1,tp1->resval.rstring,sizeof(fs1)-strlen(fs1)-1);
        } else { msgn=1; goto out; }
    }

	pc=WinExec(fs1,SW_SHOWDEFAULT);
	lsp_retint(pc);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_arx(void) {
	return(lsp_ads());
}

int lsp_ads(void) {
    int msgn;
    struct resbuf *loaded,*tp1;

    msgn=-1; loaded=NULL;

    if (lsp_argsll!=NULL) { msgn=9; goto out; }

    lsp_freesplrb(&lsp_evres);  /* Default result is nil. */

    if ((loaded=sds_loaded())!=NULL) {
        lsp_freesplrb(&lsp_evres);  /* IMPORTANT: Free prev value. */
        lsp_evres.restype=RTLB;
        lsp_evres.resval.rstring=NULL;
        lsp_evres.rbnext=loaded; loaded=NULL;  /* Just steal it. */
        tp1=lsp_evres.rbnext; while (tp1->rbnext!=NULL) tp1=tp1->rbnext;
        if ((tp1->rbnext=db_alloc_resbuf())==
            NULL) { msgn=7; goto out; }
        tp1->rbnext->restype=RTLE;
        tp1->rbnext->resval.rstring=NULL;
    }

out:
    if (loaded!=NULL) { lsp_freesuprb(loaded); loaded=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}


int lsp_load(void)      { return lsp_loader(0); }
int lsp_xload(void)     { return lsp_loader(1); }
int lsp_arxload(void)   { return lsp_loader(1); }
int lsp_xunload(void)   { return lsp_loader(2); }
int lsp_arxunload(void) { return lsp_loader(2); }

int lsp_loader(int mode) {
    char *afs1,*afs2,*readbuf;
	CString	ErrPrompt;
	TCHAR err[IC_ACADBUF];
    int pc,msgn,failed,already,fi1,fi2;
    struct resbuf *argsll,*argp[2],errnorb;
    struct _stat statbuf;
    FILE *fptr;

    struct lsp_pendlink savpend;

    msgn=-1; argsll=NULL; fptr=NULL; afs1=afs2=NULL; pc=failed=0;

    if ((afs1= new char [IC_ACADBUF])==NULL ||
        (afs2= new char [IC_ACADBUF])==NULL) { msgn=7; goto out; }

    if (lsp_argsll==NULL) { msgn=2; goto out; }

    /* Steal lsp_argsll for use as a local (because the recursion call to */
    /* lsp_lispev() in the file-processing loop would destroy it): */
    argsll=lsp_argsll; lsp_argsll=NULL;

    if ((argp[0]=argsll)==NULL)  { msgn=2; goto out; }
    if (argp[0]->restype!=RTSTR) { msgn=1; goto out; }
    if ((argp[1]=lsp_nextarg(argp[0]))!=NULL && lsp_nextarg(argp[1])!=NULL)
        { msgn=9; goto out; }

    if (mode==0) {  /* LOAD */
        int evret,savpendflag,ch,dch,mask,rbidx,remcr,done;

        readbuf=NULL; savpendflag=0; evret=0;

        /* Set extension to .lsp if there isn't one: */
        strncpy(afs1,argp[0]->resval.rstring,IC_ACADBUF-5);
        for (fi1=0; afs1[fi1]; fi1++);
        for (fi2=fi1,fi1--; fi1>-1 && !afs1[fi2] &&
            afs1[fi1]!=IC_SLASH && afs1[fi1]!=IC_OPSLASH; fi1--)
                if (afs1[fi1]=='.'/*DNT*/) fi2=fi1;
        if (!afs1[fi2]) strcat(afs1,".lsp"/*DNT*/);

        /* Try to open it: */
		// Modified Cybage AP 19/12/2000 [
		// Reason: To fix Bug No.59435
		sds_findfile(afs1,afs2);
        if ((fptr=fopen(afs2,"rb"/*DNT*/))==NULL)
		{
	           failed=1;
        } else {
			strcpy(afs1,afs2);
		// Modified Cybage AP 19/12/2000 ]
            /* The file is open at this point. */

            /* Save the current lsp_pend and and start with */
            /* a blank one for the recursion calls to lsp_lispev() */
            /* for each line of the file.  MUST BE RESTORED LATER!: */
            savpend=lsp_pend;  /* Struct assignment */
            savpendflag=1;
            /* Init it but DON'T FREE expr!  The copy points to it. */
            memset(&lsp_pend,0,sizeof(lsp_pend));
   			
			/* Just incase? */
			statbuf.st_size=IC_ACADBUF;
			
			/* Create a buffer the same size as the file we are loading */
            /* (plus 1 extra for the terminating 0 in case it doesn't end */
            /* with '\n'): */
            if(_stat(afs1,&statbuf)) { msgn=7; goto load0out; }
            if((readbuf= new char [statbuf.st_size+1])==NULL) { msgn=7; goto load0out; }

            /* See if it's protected: */
            mask=-1;  /* Not protected */
            if (fread(readbuf,sizeof(char),31,fptr)==31 &&
                strnsame(readbuf,"AutoCAD PROTECTED LISP file\r\n\x1A"/*DNT*/,30)) {

				g_lsp_bProtectedLisp = true;
                mask=(unsigned char)readbuf[30];
            } else 
				{
					// Modified Cybage AP 19/12/2000 [
					// Reason: To fix Bug No.59435
					if (fptr!=NULL) { fclose(fptr); fptr=NULL; }
					if((fptr=fopen(afs2,"rt"/*DNT*/))==NULL)
					{
						failed=1;
						goto load0out;
					}
					g_lsp_bProtectedLisp = false;
					// Modified Cybage AP 19/12/2000 ]
				}
            /* Read the file and pass the lines to lsp_lispev() (recursion): */
            done=0;
            while (!done) {
                rbidx=0;
                for (;;) {

                    if ((ch=getc(fptr))==EOF || ch=='\x1A'/*DNT*/)
                        { 
						readbuf[rbidx]=0; 
						done=1; 
						break; 
						}

                    if (ch=='\r'/*DNT*/) {

                        /* I don't think a lone '\r' is possible in a */
                        /* protected file (before decryption). */

                        /* Try for the '\n': */
                        if ((ch=getc(fptr))==EOF || ch=='\x1A'/*DNT*/) {
                            readbuf[rbidx++]='\r'/*DNT*/; 
							readbuf[rbidx]=0;
                            done=1; 
							break;

                        }

                        /* If followed by anything other than '\n', */
                        /* take the '\r' and continue with the ch just read: */
                        if (ch!='\n'/*DNT*/) 
							readbuf[rbidx++]='\r'/*DNT*/;
                    }
                    if (mask<0 || (dch=ch^mask)=='\r'/*DNT*/ || dch=='\x1A'/*DNT*/) 
						dch=ch;
                    readbuf[rbidx++]=(char)dch;
                    if (mask>-1) 
						mask=((ch<<1)&0xFF)+((ch&0x80)!=0);
                    if (dch=='\n'/*DNT*/) 
						{ 
						readbuf[rbidx]=0; 
						break; 
						}
                }

                /* rbidx is now the position of the terminal 0. */
                /* Set it to the last char: */
                if (rbidx>0) rbidx--;

                /* This and the if that checks remcr seemed to have been */
                /* added to handle true '\r's in LISP files. */
                if(readbuf[rbidx]=='\r'/*DNT*/) 
					{ 
					readbuf[rbidx]='\n'/*DNT*/; 
					remcr=0; 
					}

                if((evret=lsp_lispev(readbuf,NULL,lsp_fnvarval,&pc))!=0) 
					break;

				if(remcr && lsp_pend.exprlen>0 && lsp_pend.expr[lsp_pend.exprlen-1]=='\n'/*DNT*/) 
					{
		            lsp_pend.expr[lsp_pend.exprlen-1]=0;
		            --lsp_pend.exprlen;
				}
			}

        }  /* End else it's open */

      load0out:

        if (readbuf!=NULL) { IC_FREE(readbuf);  readbuf=NULL; }

        /* Free lsp_pend and put it back like it was: */
        if (savpendflag) {
            lsp_freepend();  /* Free the one we just used for the file. */
            lsp_pend=savpend;  /* Struct assignment */
        }

        if (msgn>-1) goto out;

        if (evret) {
            if (argp[1]!=NULL) failed=1;
            else msgn=-2;
        } else if (pc) { msgn=13; goto out; }  /* Report unmatched parentheses */

    } else if (mode==1 || mode==2) {  /* XLOAD and XUNLOAD */

        if (((mode==1) ? sds_xload(argp[0]->resval.rstring) :
                      sds_xunload(argp[0]->resval.rstring))!=RTNORM) failed=1;
        else if (lsp_retstr(argp[0]->resval.rstring)!=RTNORM)
            { msgn=7; goto out; }

    }

    if (failed) {
        /* Had to go to some pains here to match AutolISP's reactions. */

        already=0;  /* Already loaded flag for XLOAD. */

        /* See if already xloaded: */
        if (mode==1 && (already=(sds_getvar("ERRNO"/*DNT*/,&errnorb)==RTNORM &&
            errnorb.resval.rint==OL_ELOADED))!=0) {

            /* THIS one is printed whether or not ONFAILURE was supplied: */
			ErrPrompt = ResourceString(IDC_APP_LISP_APPLICATION_S_0, "\nApplication \"%s\" already loaded\n" );
			_snprintf(err, sizeof(err), ErrPrompt, argp[0]->resval.rstring);
            sds_printf(err);

            /* Set result to appname as if succeeded (may be over-ridden */
            /* by ONFAILURE below): */
            if (lsp_retstr(argp[0]->resval.rstring)!=RTNORM)
                { msgn=7; goto out; }
        }

        if (argp[1]!=NULL) {
            /* Take the onfailure arg as the result (note that */
            /* evaluation has NOT been delayed; the arg is ALWAYS */
            /* evaluated -- but only set as the result upon failure): */

            if (lsp_retval(argp[1])!=RTNORM) { msgn=7; goto out; }

        } else {
            /* The rest of the standard messages, given only when */
            /* ONFAILURE is NOT supplied: */
            if      (mode==0) {
				ErrPrompt = ResourceString(IDC_APP_LISP_CAN_T_OPEN_INPUT_1, "\nCan't open \"%s\" for input\n");
				_snprintf(err, sizeof(err), ErrPrompt, afs1);
				sds_printf(err);
                msgn=12;
            } else if (mode==1) {
                if (!already) {
				ErrPrompt = ResourceString(IDC_APP_LISP_CAN_T_OPEN_EXE_2, "\nCan't open \"%s\" for execution\n");
				_snprintf(err, sizeof(err), ErrPrompt, argp[0]->resval.rstring);
				sds_printf(err);
                    msgn=50;
                }
            } else if (mode==2) {
				ErrPrompt = ResourceString(IDC_APP_LISP_CAN_T_APP_S_NOTLOADED_3, "\nApplication \"%s\" not loaded\n");
				_snprintf(err, sizeof(err), ErrPrompt, argp[0]->resval.rstring);
				sds_printf(err);
                msgn=51;
            }
        }
        goto out;  /* In case we add more code below someday. */
    }

out:
    lsp_freesuprb(argsll); argsll=NULL;  /* The original lsp_argsll. */
    if (fptr!=NULL) { fclose(fptr); fptr=NULL; }
    if (afs1!=NULL) { IC_FREE(afs1); afs1=NULL; }
    if (afs2!=NULL) { IC_FREE(afs2); afs2=NULL; }

    if (msgn!=-1) {  /* May be -2 from bad lsp_evalarg() or lsp_lispev(). */
        if (msgn>-1) lsp_fnerrmsg(msgn);
        return -1;
    }
    return 0;
}



