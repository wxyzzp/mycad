/* C:\ICAD\PRJ\LIB\LISP\FILEIO.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

#include "lisp.h"/*DNT*/

int lsp_read_line(void) {
/*
**  This function mimics the following AutoLISP read-line characterics:
**
**      Text and binary modes are treated the same (so we have lsp_open()
**      always opening in binary for reading).
**
**      \r, \n, \r\n, and \n\r are equivalent line-terminators
**      and do not get placed in the result string.
**
**      ^Z (char 26) if a file-terminator.
**
**      Char 0 causes the reader to skip chars until it has eaten
**      a line terminator; it then starts collecting chars again,
**      as normal.
*/
    char cr,lf,cz,*ln;
    int lnsz,lnsz_1,bigsz,pass,lnidx,skip,firstget,msgn;
    int ch,tch;  /* Not char; EOF is -1 */
    long seeker;
    FILE *fptr;
    struct lsp_fileplink *tfp1;

    msgn=-1; fptr=NULL; ln=NULL; seeker=0L; ch=0; firstget=1;
    cr='\r'/*DNT*/; lf='\n'/*DNT*/; cz='\x1A'/*DNT*/;

    bigsz=65536;  /* Size to expand ln to (if needed) */

    /* Start with a normal size buffer: */
    lnsz=IC_ACADBUF;
    if ((ln= new char [ lnsz] )==NULL) { msgn=7; goto out; }

    if (lsp_argsll!=NULL) {
        if (lsp_argsll->restype!=LSP_RTFILE) { msgn=1; goto out; }
        if (lsp_argsll->rbnext!=NULL)        { msgn=9; goto out; }
        fptr=(FILE *)lsp_argsll->resval.rstring;

        /* See if it's in our llist of open file pointers: */
        for (tfp1=lsp_filepll; tfp1!=NULL && tfp1->fp!=fptr; tfp1=tfp1->next);
        if (tfp1==NULL) { msgn=14; goto out; }
    }

    if (fptr==NULL) 
		{
		if(RTNORM!=sds_getstring(0,NULL,ln))
			{ 
			lsp_retnil(); 
			goto out; 
			}
		}
	else 
		{  /* Read from fptr. */

        for (pass=0; pass<2; pass++) {  /* Try using int buffer 1st */

            if (pass) {  /* Make the buffer bigger and try again */
                IC_FREE(ln); ln=NULL; lnsz=bigsz;
                if ((ln= new char [ lnsz ])==NULL) { msgn=7; goto out; }
                fseek(fptr,seeker,SEEK_SET);
            } else seeker=ftell(fptr);

            *ln=0; lnidx=0; skip=0; lnsz_1=lnsz-1; ch=0; firstget=1;

            for (;;) {

                if (lnidx>=lnsz_1) break;
                if ((ch=getc(fptr))==EOF || ch==cz) break;
                firstget=0;

                if (ch==cr || ch==lf) {
                    if ((tch=getc(fptr))!=EOF &&  /* OK to unget cz */
                        tch!=((ch==cr) ? lf : cr)) ungetc(tch,fptr);
                    if (!skip) break;
                    skip=0;
                } else if (ch==0) {
                    skip=1;
                } else {
                    if (!skip) ln[lnidx++]=ch;
                }

            }

            if (lnidx<lnsz_1) break;

        }  /* End for pass */

        if (ferror(fptr)) { clearerr(fptr); msgn=15; goto out; }

        if (lnidx>=lnsz) lnidx=lnsz_1;  /* Precaution */
        ln[lnidx]=0;

        if (ch==cz) { fseek(fptr,0L,SEEK_END); ch=EOF; }
        /* (Must detect EOF next time.) */

        /* If very 1st char read was eof, return nil: */
        if (firstget && ch==EOF) { lsp_retnil(); goto out; }

    }

    if (lsp_retstr(ln)!=RTNORM) { msgn=7; goto out; }

out:
    if (ln!=NULL) { IC_FREE(ln); ln=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_read_char(void) {
/*
**  This function mimics the following AutoLISP read-char characterics:
**
**      Text and binary modes are treated the same (so we have lsp_open()
**      always opening in binary for reading).
**
**      \r, \n, \r\n, and \n\r are equivalent line-terminators
**      and all return 10 (\n).
**
**      ^Z (char 26) if a file-terminator (returns nil, not 26).
**
**      Char 0 is read like any other char.
*/
    char cr,lf,cz;
    int msgn;
    int ch,tch;  /* Not char; EOF is -1 */
    FILE *fptr;
    struct lsp_fileplink *tfp1;

    msgn=-1; fptr=NULL; ch=0;
    cr='\r'/*DNT*/; lf='\n'/*DNT*/; cz='\x1A'/*DNT*/;

    if (lsp_argsll!=NULL) {
        if (lsp_argsll->restype!=LSP_RTFILE) { msgn=1; goto out; }
        if (lsp_argsll->rbnext!=NULL)        { msgn=9; goto out; }
        fptr=(FILE *)lsp_argsll->resval.rstring;

        /* See if it's in our llist of open file pointers: */
        for (tfp1=lsp_filepll; tfp1!=NULL && tfp1->fp!=fptr; tfp1=tfp1->next);
        if (tfp1==NULL) { msgn=14; goto out; }
    }

    if (fptr==NULL) {
        /* Get from keyboard */
		char fs1[IC_ACADBUF];
		if(RTNORM==sds_getstring(0,NULL,fs1)) ch=(*fs1==0) ? '\n'/*DNT*/ : *fs1;
		else { lsp_retnil(); goto out; }
    } else {  /* Read from fptr. */

        if ((ch=getc(fptr))==EOF || ch==cz) {
            if (ch==cz) fseek(fptr,0L,SEEK_END); /* Must see EOF next time */
            lsp_retnil(); goto out;
        }

        if (ferror(fptr)) { msgn=15; goto out; }

        if (ch==cr || ch==lf) {
            if ((tch=getc(fptr))!=EOF && tch!=((ch==cr) ? lf : cr))
                ungetc(tch,fptr);  /* cz ok to unget */
            ch=lf;
        }

    }

    lsp_retint(ch);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_write_line(void) {
/*
**  Like the AutoLISP write-line, this function converts the line-
**  terminator \n\r to \r (in text and binary mode).  The other
**  line-terminators (\n, \r, \r\n) are written out as normal for
**  the mode (meaning \n is converted to \r\n and \r\n is converted to
**  \r\r\n in text mode).
**
**  A final \n (binary mode) or \r\n (text mode) is appended.
*/
    char *afs1,*tosend;
    int msgn;
    FILE *fptr;
    struct lsp_fileplink *tfp1;

    msgn=-1; fptr=NULL; afs1=NULL;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL) {
        if (lsp_argsll->rbnext->restype!=LSP_RTFILE) { msgn=1; goto out; }
        if (lsp_argsll->rbnext->rbnext!=NULL)        { msgn=9; goto out; }
        fptr=(FILE *)lsp_argsll->rbnext->resval.rstring;

        /* See if it's in our llist of open file pointers: */
        for (tfp1=lsp_filepll; tfp1!=NULL && tfp1->fp!=fptr; tfp1=tfp1->next);
        if (tfp1==NULL) { msgn=14; goto out; }
    }

    if (strstr(lsp_argsll->resval.rstring,"\n\r"/*DNT*/)==NULL)
        tosend=lsp_argsll->resval.rstring;
    else {  /* Possible \n\r line-terminator to convert */
        if ((afs1= new char [strlen(lsp_argsll->resval.rstring)+1])==NULL)
            { msgn=7; goto out; }
        strcpy(afs1,lsp_argsll->resval.rstring);
        lsp_convlfcr(afs1);  /* Convert \n\r string terminator to \r */
        tosend=afs1;
    }

    if (fptr==NULL) sds_printf("%s\n"/*DNT*/,lsp_argsll->resval.rstring);
    else fprintf(fptr,"%s\n"/*DNT*/,tosend);  /* Converted only goes to file */

    /* Return the original: */
    if (lsp_retstr(lsp_argsll->resval.rstring)!=RTNORM) { msgn=7; goto out; }

out:
    if (afs1!=NULL) { IC_FREE(afs1); afs1=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_write_char(void) {
    int msgn;
    FILE *fptr;
    struct lsp_fileplink *tfp1;

    msgn=-1; fptr=NULL;

    if (lsp_argsll==NULL)            { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTLONG) { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL) {
        if (lsp_argsll->rbnext->restype!=LSP_RTFILE) { msgn=1; goto out; }
        if (lsp_argsll->rbnext->rbnext!=NULL)        { msgn=9; goto out; }
        fptr=(FILE *)lsp_argsll->rbnext->resval.rstring;

        /* See if it's in our llist of open file pointers: */
        for (tfp1=lsp_filepll; tfp1!=NULL && tfp1->fp!=fptr; tfp1=tfp1->next);
        if (tfp1==NULL) { msgn=14; goto out; }
    }

    if (lsp_argsll->resval.rlong) {
        if (fptr==NULL) sds_printf("%c"/*DNT*/,lsp_argsll->resval.rlong);
        else fprintf(fptr,"%c"/*DNT*/,lsp_argsll->resval.rlong);
    }

    lsp_retint(lsp_argsll->resval.rlong);

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_open(void) {
    char *mode,*fcp1;
    int msgn;
    struct resbuf retrb;
    struct lsp_fileplink *newlink;

    msgn=-1; newlink=NULL; retrb.restype=RTNIL; retrb.rbnext=NULL;

    if (lsp_argsll==NULL || lsp_argsll->rbnext==NULL) { msgn=2; goto out; }
    if (lsp_argsll->rbnext->rbnext!=NULL)             { msgn=9; goto out; }
    if (lsp_argsll->restype!=RTSTR || lsp_argsll->rbnext->restype!=RTSTR)
        { msgn=1; goto out; }

    if ((newlink= new struct lsp_fileplink )==NULL) { msgn=7; goto out; }
	memset(newlink,0,sizeof(struct lsp_fileplink));

    /* If READING, open in BINARY (so we can mimic AutoLISP's */
    /* read-line and read-char line and file termination quirks): */
    fcp1=lsp_argsll->rbnext->resval.rstring;
	// Note fopen will fail if the mode is upper case!
	strlwr(fcp1);
	// Note, We do NOT allow (acad either) any combined modes as in r+w. LISP
	// allows only "r" "w" and "a". If 'reading' change "r" to "rb". If we get something
	// longer that one character, we are going to check ONLY the first character and 
	// make sure only that one character gets through.
	// We already get a "error: bad argument type" if any mode other than r,w,a is used.
	switch(*fcp1)
		{
		case 'r':
			mode = "rb";
			break;
		case 'w':
			mode = "w";
			break;
		case 'a':
			mode = "a";
			break;
		default:
			mode = lsp_argsll->rbnext->resval.rstring;
			break;
		}

    if ((newlink->fp=fopen(lsp_argsll->resval.rstring,mode))==NULL)
        retrb.restype=RTNIL;
    else {
        retrb.restype=LSP_RTFILE;
        retrb.resval.rstring=(char *)newlink->fp;
        newlink->next=lsp_filepll; lsp_filepll=newlink;
        newlink=NULL;
    }

    if (lsp_retval(&retrb)!=RTNORM) { msgn=7; goto out; }

out:
    if (newlink!=NULL) { delete newlink; newlink=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_close(void) {
    int msgn;
    struct lsp_fileplink *tfp1,*before;
    FILE *fptr;

    msgn=-1;

    if (lsp_argsll==NULL)                { msgn=2; goto out; }
    if (lsp_argsll->rbnext!=NULL)        { msgn=9; goto out; }
    if (lsp_argsll->restype!=LSP_RTFILE) { msgn=1; goto out; }

    if ((fptr=(FILE *)lsp_argsll->resval.rstring)==NULL)
        { msgn=14; goto out; }

    /* Find it in the llist of open file pointers: */
    for (before=NULL,tfp1=lsp_filepll; tfp1!=NULL &&
        tfp1->fp!=fptr; before=tfp1,tfp1=tfp1->next);

    if (tfp1==NULL) { msgn=14; goto out; }

    /* Close it and remove the link from lsp_filepll: */
    fclose(fptr);
    if (before==NULL) lsp_filepll=tfp1->next;
    else             before->next=tfp1->next;
    delete tfp1;

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_findfile(void) {
    char *afs1;
    int msgn;

    msgn=-1; afs1=NULL;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR || lsp_argsll->resval.rstring==NULL)
                                    { msgn=1; goto out; }
    if (lsp_argsll->rbnext!=NULL)   { msgn=9; goto out; }

    if ((afs1= new char [ 512])==NULL) { msgn=7; goto out; }

    if (sds_findfile(lsp_argsll->resval.rstring,afs1)==RTNORM) {
        if (lsp_retstr(afs1)!=RTNORM) { msgn=7; goto out; }
    } else lsp_retnil();

out:
    if (afs1!=NULL) { IC_FREE(afs1); afs1=NULL; }
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

