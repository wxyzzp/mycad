/* C:\ICAD\PRJ\LIB\LISP\CONSOLEIO.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// THIS FILE HAS BEEN GLOBALIZED!

//** Includes
#include "lisp.h"/*DNT*/
#include "commandatom.h" /*DNT*/
#include <stdio.h>

#define NEW_LSP_PRTRECUR

int lsp_alert(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)                 { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR ||
        lsp_argsll->resval.rstring==NULL) { msgn=1; goto out; }
    /* Too many is okay; it only looks at the first. */

    sds_alert(lsp_argsll->resval.rstring);

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

/* No errors for this one: */
int lsp_terpri(void) { sds_printf("\n"/*DNT*/); lsp_retnil(); return 0; }

int lsp_prompt(void) {
    int msgn;

    msgn=-1;

    if (lsp_argsll==NULL)           { msgn=2; goto out; }
    if (lsp_argsll->restype!=RTSTR) { msgn=1; goto out; }
    /* Hey!  No Too many args warning this time!  They caught on! */

    sds_printf("%s"/*DNT*/,lsp_argsll->resval.rstring);

    lsp_retnil();

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}

int lsp_princ(void) { return lsp_printem(0); }
int lsp_prin1(void) { return lsp_printem(1); }
int lsp_print(void) { return lsp_printem(2); }


int lsp_printem(int mode) {
    int msgn;
    FILE *fptr;
    struct resbuf *argp[2];
    struct lsp_fileplink *tfp1;

    msgn=-1; fptr=NULL;

    if ((argp[0]=lsp_argsll)==NULL) {
        /* Oh, boy!  No args!  Time for the mystery symbol! */

        lsp_freesplrb(&lsp_evres);
        lsp_evres.restype=LSP_RTSYMB;
        if ((lsp_evres.resval.rstring= new char [2] )==NULL) 
			msgn=7;
        else 
		{
			memset(lsp_evres.resval.rstring,0,sizeof(char)*2);
			lsp_evres.resval.rstring[0]=lsp_mysterysym;
		}
        goto out;
    }

    if ((argp[1]=lsp_nextarg(argp[0]))!=NULL &&
        lsp_nextarg(argp[1])!=NULL) { msgn=9; goto out; }

    if (argp[1]!=NULL) {
        /* Check 2nd arg for type here (file pointer only) */
        /* and set fptr to the file pointer. */
        /* (Check lsp_filepll to make sure it's open.) */

        if (argp[1]->restype!=LSP_RTFILE) { msgn=1; goto out; }

        fptr=(FILE *)argp[1]->resval.rstring;

        for (tfp1=lsp_filepll; tfp1!=NULL && tfp1->fp!=fptr; tfp1=tfp1->next);
        if (tfp1==NULL) fptr=NULL; /* Not found (not open); print to screen.*/
    }

    /* Call the recursive printing routine: */
    msgn=lsp_prtrecur(argp[0],fptr,mode,0);

    if (lsp_retval(argp[0])!=RTNORM) { msgn=7; goto out; }

out:
    if (msgn>-1) { lsp_fnerrmsg(msgn); return -1; }
    return 0;
}
/*-------------------------------------------------------------------------*//**
Helper class optimizing lsp_prtrecur() implementation
*/
class CLspPrt
{
public:
	CLspPrt(FILE* fptr, int mode): m_fptr(fptr), m_mode(mode), m_rec(0)
	{
		initBuf();
		m_str = m_bufstr;
		m_strlen = sizeof(m_bufstr) - 1;
	}
	~CLspPrt(){ flush(); freeStr(); }
	int doRec(struct resbuf *rbarg, int recur);

protected:
	enum EFmt
	{
		eFbin ,//= "(%d . <Binary data>)"/*DNT*/,
		eFshort,//="%d"/*DNT*/, 
		eFlong,//= "%ld"/*DNT*/,
		eFreal,//= "%#G"/*DNT*/,
		eFstr,//=  "%s"/*DNT*/,
		eFqtstr,//="%s%s%s"/*DNT*/,
		eFnil,//=  "nil"/*DNT*/,
		eFt,//=    "T"/*DNT*/,
		eFsubr,//= "<Subr: #%p>"/*DNT*/,
		eFxsub,//= "<Ext. Subr: %d #%p>"/*DNT*/,
		eFfile,//= "<File: #%p>"/*DNT*/,
		eFename,//="<Entity name: %lx>"/*DNT*/,
		eFpicks,//="<Selection set: %ld>"/*DNT*/,
		eF2dpt,//= "(%#G %#G)"/*DNT*/,
		eF3dpt,//= "(%#G %#G %#G)"/*DNT*/
		eFlast
	};
	static const char* aFmt[eFlast];

protected:
	void print(EFmt eFmt, ...)
	{
		const char* fmt = aFmt[eFmt];
		char str[32];
		if(eFmt != eFreal && !m_rec)
		{
			assert(strlen(fmt) + 1 < sizeof(str));
			strcpy(str, fmt);
			strcat(str, " ");
			fmt = str;
		}
		va_list	args; 
		va_start(args, eFmt);
		msprintf(fmt, args);
		va_end(args);
	}

	void print(const char* str)
	{
		msprintf(str, NULL);
	}

	void initBuf()
	{
		m_nBufUsed = 0;
		m_nBufLeft = cBuffSize - m_nBufUsed - 1;
	}

	void incBuf(int n)
	{
		assert(m_nBufLeft >= n);
		m_nBufUsed += n;
        m_nBufLeft -= n;
	}

	void checkBuf(bool bForce)
	{
		if(m_nBufLeft == 0 || bForce && m_nBufUsed)
		{
			m_buf[m_nBufUsed] = '\0';
			m_stream += m_buf;
			initBuf();
		}
	}

	void flush()
	{
		const char* str = NULL;
		if(m_stream.length())
		{
			checkBuf(true);
			str = m_stream.c_str();
		}
		else if(m_nBufUsed)
		{
			m_buf[m_nBufUsed] = '\0';
			str = m_buf;
		}
		if(str)
		{
			if(m_fptr)
				fprintf(m_fptr, str);
			else 
				sds_printf(str);
			m_stream.erase();
			initBuf();
		}
	}

	void msprintf(const char* format, va_list args)
	{
		checkBuf(false);
		int nUsed = _vsnprintf(&m_buf[m_nBufUsed], m_nBufLeft, format, args);
		if(nUsed < 0)
		{
			checkBuf(true);
			nUsed = _vsnprintf(&m_buf[m_nBufUsed], m_nBufLeft, format, args);
			if(nUsed < 0)
			{
				flush();
				mprintf(format, args);
			}
		}
		if(nUsed > 0)
			incBuf(nUsed);
	}

	void mprintf(const char* format, va_list args)
	{
		if(m_fptr)
		{
			vfprintf(m_fptr, format, args);
		}
		else
		{
#if _MSC_VER >= 1300
			int size = _vscprintf(format, args);
			if(size > 0)
			{
				char* str = new char[size + 1];
				::_vsprintf(str, args);
				str[size = '\0'];
				sds_printf(str);
				delete str;
			}
#endif
		}
	}

	void freeStr()
	{
		if(m_str != m_bufstr)
		{
			delete [] m_str;
			m_str = m_bufstr;
			m_strlen = sizeof(m_bufstr) - 1;
		}
	}

	bool allocStr(int len)
	{
		if(len > m_strlen)
		{
			freeStr();
			char* str = new char [len + 1];
			if(!str)
				return false;
			m_strlen = len;
			m_str = str;
		}
		return true;
	}

protected:
	FILE* m_fptr;
	int m_mode;
	int m_rec;

	//output buffer
	std::string m_stream;
	enum EBuffSize
	{
		cBuffSize = IC_ACADBUF
	};
	char m_buf[cBuffSize];
	int m_nBufUsed;
	int m_nBufLeft;

	//internal string
	char m_bufstr[1024];
	char* m_str;
	int m_strlen;
};

const char* CLspPrt::aFmt[eFlast] =
{
	"(%d . <Binary data>)"/*DNT*/,
	"%d"/*DNT*/,
	"%ld"/*DNT*/,
	"%#G"/*DNT*/,
	"%s"/*DNT*/,
	"%s%s%s"/*DNT*/,
	"nil"/*DNT*/,
	"T"/*DNT*/,
	"<Subr: #%p>"/*DNT*/,
	"<Ext. Subr: %d #%p>"/*DNT*/,
	"<File: #%p>"/*DNT*/,
	"<Entity name: %lx>"/*DNT*/,
	"<Selection set: %ld>"/*DNT*/,
	"(%#G %#G)"/*DNT*/,
	"(%#G %#G %#G)"/*DNT*/
};

int CLspPrt::doRec(struct resbuf *rbarg, int recur)
{
	/* Statics: */
    static const char *spstr=" "/*DNT*/;
	m_rec = recur;
    int msgn=-1;

    if(!recur) {  /* Initial call */

        /* For print, send the initial linefeed: */
        if(m_mode==2)
			print("\n"/*DNT*/);
    }
    switch (rbarg->restype) {
        case RTSTR:

            // Don't print the special chars from (princ)
            if(*rbarg->resval.rstring==lsp_mysterysym) break;

            // EBATECH-[2001.3.26 ICAD.LSP load string is not print
            if(strstr(rbarg->resval.rstring,"(if (/= s::startup nil)(s::startup))(princ)") != NULL)
				return msgn;
            // ]-EBATECH

            if(m_mode==0) 
			{
				/* princ */
				int len = strlen(rbarg->resval.rstring);
				if(!allocStr(len))
					return 7;
                strcpy(m_str,rbarg->resval.rstring);
                lsp_convlfcr(m_str);  /* Convert \n\r to \r */
            }
			else
			{
				/* prin1 and print (convert to esc seqs) */
				int len = ic_escconv(rbarg->resval.rstring,NULL,
                    '\\'/*DNT*/,lsp_true2esc,1,1);
				if(!allocStr(len))
					return 7;
                ic_escconv(rbarg->resval.rstring, m_str, '\\'/*DNT*/,lsp_true2esc,1,1);
            }

            if(strcmp(m_str, "\\"/*DNT*/) != 0)
			{
				const char* fcp1=(m_mode) ? "\""/*DNT*/ : ""/*DNT*/;  /* Add quote for prin1 and print. */
				print(eFqtstr, fcp1, m_str, fcp1);
			}
            break;
        case RTLONG:
            print(eFlong,rbarg->resval.rlong);
            break;
        case RTPOINT:
            print(eF2dpt,rbarg->resval.rpoint[0],rbarg->resval.rpoint[1]);
            break;
        case RT3DPOINT:
            print(eF3dpt,rbarg->resval.rpoint[0],rbarg->resval.rpoint[1],rbarg->resval.rpoint[2]);
            break;
        case RTREAL: case RTANG: case RTORINT:
            sprintf(m_str, aFmt[eFreal], rbarg->resval.rreal);
            {
				/* Add .0 if no 'E', 'e', or decimal pt found: */
				char* fcp1 = m_str;
				while (*fcp1 && *fcp1!='.'/*DNT*/ && toupper(*fcp1)!='E'/*DNT*/) fcp1++;
				if (!*fcp1) { *fcp1++='.'/*DNT*/; *fcp1++='0'/*DNT*/; *fcp1=0; }
			}
            print(eFstr, m_str);
            break;
        case RTNIL:
            print(eFnil);
            break;
        case RTT:
            print(eFt);
            break;
        case LSP_RTSUBR: case LSP_RTFILE:
            print((rbarg->restype==LSP_RTSUBR) ? eFsubr : eFfile, rbarg->resval.rstring);
            break;
        case LSP_RTXSUB:
            /* Find the atom by fn ptr to get the ID number */
            /* (/// THIS WILL CHANGE WHEN WE REMOVE IDs FROM THE */
            /* ATOMS LIST): */
            {
				class commandAtomObj* tap1 = lsp_findatom(rbarg->resval.rstring,1);
				int fi1 = (tap1==NULL) ? -9999 /* Can't find??? */ : tap1->id;
				print(eFxsub, fi1, rbarg->resval.rstring);
			}
            break;
        case RTSHORT:
            print(eFshort, rbarg->resval.rint);
            break;
        case RTLB:
            /* If it's (), print nil: */
            if (rbarg->rbnext!=NULL && rbarg->rbnext->restype==RTLE) {
				print(eFnil);
                break;
            }

            /* Start with '(': */
            print("("/*DNT*/);
			{
				/* Do the contents by calling this function recursively: */
				for(resbuf* tp1=rbarg->rbnext, *tp2 = NULL; tp1!=NULL && tp1->restype!=RTLE; tp1=tp2) {

					if (sds_usrbrk())
						return 20;

					const int saveRec = m_rec;
					msgn = doRec(tp1, 1);
					m_rec = saveRec;
					if(msgn > -1)
						return msgn;

					/* If it's not the last arg in the list, add a space: */
					if ((tp2=lsp_nextarg(tp1))!=NULL && tp2->restype!=RTLE) {
						print(spstr);
					}
				}
			}
            /* End with ')': */
			// Modified CyberAge PP 08/22/2000 [
			// Fix for bug no. 2488
			print(eFstr, ")"/*DNT*/);
			// Modified CyberAge PP 08/22/2000 ]
            break;
        case LSP_RTSYMB:
            /* Okay, now, don't print the mystery symbol!: */
            if (rbarg->resval.rstring[0]==lsp_mysterysym &&
                rbarg->resval.rstring[1]==0) break;
            print(eFstr, rbarg->resval.rstring);
            break;
        case RTDOTE:
            print("."/*DNT*/);
            break;
        case RTENAME: case RTPICKS:
            {
				int fi1=0;
				print((rbarg->restype==RTPICKS) ? eFpicks : eFename, rbarg->resval.rlname[fi1]);
			}
			break;
        case 1004: case RTBINARY:
            print(eFbin, rbarg->restype);
            break;
    }
    return msgn;
}

#ifdef NEW_LSP_PRTRECUR
int lsp_prtrecur(struct resbuf *rbarg, FILE *fptr, int mode, int recur)
{
	CLspPrt prt(fptr, mode);
	return prt.doRec(rbarg, recur);
}

#else


int lsp_prtrecur(struct resbuf *rbarg, FILE *fptr, int mode, int recur) {
/*
**  THIS FUNCTION IS AN EXTENSION OF lsp_printem().
**
**  This function prints one lisp arg.  If that arg is a list, it
**  calls itself recursively to print the members of the list.
**
**  rbarg : A pointer to the arg to print.
**   fptr : File pointer.  If NULL the printing goes to the screen.
**   mode : The lsp_printem() mode (0=princ; 1=prin1; 2=print).
**  recur : The first call must pass 0; recursion calls must pass 1.
**            It's is a simple way for this function to tell the
**            outer-most call from the recursions.  (Could use a
**            static counter, but that's bug-prone if the incrementing/
**            decrementing isn't handled perfectly.
**
**  Returns: An lsp_fnerrmsg() code (-1=OK; >-1=error number)
**           (but doesn't call it).
*/
    char *fcp1;
    int pass,msgn,fi1;
    long len;
    struct resbuf *tp1,*tp2;
	class commandAtomObj *tap1;

                             /* Statics: */

    static char *ststr;
    static long ststrsz;

    static char *spstr=" "/*DNT*/;

	/* Printing formats: */
	// Modified CyberAge PP 08/22/2000 [
	// Fix for bug no. 2488
	static char
		*fbin ,//= "(%d . <Binary data>)"/*DNT*/,
		*fshort,//="%d"/*DNT*/, 
		*flong,//= "%ld"/*DNT*/,
		*freal,//= "%#G"/*DNT*/,
		*fstr,//=  "%s"/*DNT*/,
		*fqtstr,//="%s%s%s"/*DNT*/,
		*fnil,//=  "nil"/*DNT*/,
		*ft,//=    "T"/*DNT*/,
		*fsubr,//= "<Subr: #%p>"/*DNT*/,
		*fxsub,//= "<Ext. Subr: %d #%p>"/*DNT*/,
		*ffile,//= "<File: #%p>"/*DNT*/,
		*fename,//="<Entity name: %lx>"/*DNT*/,
		*fpicks,//="<Selection set: %ld>"/*DNT*/,
		*f2dpt,//= "(%#G %#G)"/*DNT*/,
		*f3dpt;//= "(%#G %#G %#G)"/*DNT*/;

	if(!recur)
	{
		fbin = "(%d . <Binary data>) "/*DNT*/;
		fshort="%d "/*DNT*/;
		flong= "%ld "/*DNT*/;
		freal= "%#G"/*DNT*/;
		fstr=  "%s "/*DNT*/;
		fqtstr="%s%s%s "/*DNT*/;
		fnil=  "nil "/*DNT*/;
		ft=    "T "/*DNT*/;
		fsubr= "<Subr: #%p> "/*DNT*/;
		fxsub= "<Ext. Subr: %d #%p> "/*DNT*/;
		ffile= "<File: #%p> "/*DNT*/;
		fename="<Entity name: %lx> "/*DNT*/;
		fpicks="<Selection set: %ld> "/*DNT*/;
		f2dpt= "(%#G %#G) "/*DNT*/;
		f3dpt= "(%#G %#G %#G) "/*DNT*/;
	}
	else
	{
		fbin = "(%d . <Binary data>)"/*DNT*/;
		fshort="%d"/*DNT*/;
		flong= "%ld"/*DNT*/;
		freal= "%#G"/*DNT*/;
		fstr=  "%s"/*DNT*/;
		fqtstr="%s%s%s"/*DNT*/;
		fnil=  "nil"/*DNT*/;
		ft=    "T"/*DNT*/;
		fsubr= "<Subr: #%p>"/*DNT*/;
		fxsub= "<Ext. Subr: %d #%p>"/*DNT*/;
		ffile= "<File: #%p>"/*DNT*/;
		fename="<Entity name: %lx>"/*DNT*/;
		fpicks="<Selection set: %ld>"/*DNT*/;
		f2dpt= "(%#G %#G)"/*DNT*/;
		f3dpt= "(%#G %#G %#G)"/*DNT*/;
	}
	// Modified CyberAge PP 08/22/2000 ]

    msgn=-1; fcp1=NULL;

    if (!recur) {  /* Initial call */

        /* Start ststr with a size that will handle all small vals: */
        ststrsz=IC_ACADBUF;
        if (ststr!=NULL) IC_FREE(ststr);  /* Should never, but... */
        if ((ststr= new char [ststrsz])==NULL) { msgn=7; goto out; }

        /* For print, send the initial linefeed: */
        if (mode==2) {
            fcp1="\n"/*DNT*/;
            if (fptr!=NULL) fprintf(fptr,fcp1); else sds_printf(fcp1);
        }

    }

    switch (rbarg->restype) {
        case RTSTR:

            // Don't print the special chars from (princ)
            if(*rbarg->resval.rstring==lsp_mysterysym) break;

            // EBATECH-[2001.3.26 ICAD.LSP load string is not print
            if(strstr(rbarg->resval.rstring,"(if (/= s::startup nil)(s::startup))(princ)") != NULL) goto out;
            // ]-EBATECH

            len=0;  /* Length of the string. */
            for (pass=0; pass<2; pass++) {
                /* 0: find len; 1: alloc and build it */

                if (pass && len>=ststrsz) {
                    IC_FREE(ststr); ststr=NULL; ststrsz=len+1;
                    if ((ststr= new char [ststrsz])==NULL)
                        { msgn=7; goto out; }
                }


                if (mode==0) {  /* princ */
                    if (pass) {
                        strcpy(ststr,rbarg->resval.rstring);
                        lsp_convlfcr(ststr);  /* Convert \n\r to \r */
                    } else len=strlen(rbarg->resval.rstring);
                } else {  /* prin1 and print (convert to esc seqs) */
                    if (pass) ic_escconv(rbarg->resval.rstring,ststr,
                        '\\'/*DNT*/,lsp_true2esc,1,1);
                    else  len=ic_escconv(rbarg->resval.rstring,NULL,
                        '\\'/*DNT*/,lsp_true2esc,1,1);
                }
            }

            fcp1=(mode) ? "\""/*DNT*/ : ""/*DNT*/;  /* Add quote for prin1 and print. */
            if (strcmp(ststr, "\\"/*DNT*/) != 0) {
                if (fptr!=NULL) fprintf(fptr,fqtstr,fcp1,ststr,fcp1);
                else              sds_printf(fqtstr,fcp1,ststr,fcp1);
            }
            break;
        case RTLONG:
            if (fptr!=NULL) fprintf(fptr,flong,rbarg->resval.rlong);
            else              sds_printf(flong,rbarg->resval.rlong);
            break;
        case RTPOINT:
            if (fptr!=NULL) fprintf(fptr,f2dpt,rbarg->resval.rpoint[0],rbarg->resval.rpoint[1]);
            else              sds_printf(f2dpt,rbarg->resval.rpoint[0],rbarg->resval.rpoint[1]);
            break;
        case RT3DPOINT:
            if (fptr!=NULL) fprintf(fptr,f3dpt,rbarg->resval.rpoint[0],rbarg->resval.rpoint[1],rbarg->resval.rpoint[2]);
            else              sds_printf(f3dpt,rbarg->resval.rpoint[0],rbarg->resval.rpoint[1],rbarg->resval.rpoint[2]);
            break;
        case RTREAL: case RTANG: case RTORINT:
            sprintf(ststr,freal,rbarg->resval.rreal);
            /* Add .0 if no 'E', 'e', or decimal pt found: */
            fcp1=ststr;
            while (*fcp1 && *fcp1!='.'/*DNT*/ && toupper(*fcp1)!='E'/*DNT*/) fcp1++;
            if (!*fcp1) { *fcp1++='.'/*DNT*/; *fcp1++='0'/*DNT*/; *fcp1=0; }
            if (fptr!=NULL) fprintf(fptr,fstr,ststr);
            else              sds_printf(fstr,ststr);
            break;
        case RTNIL:
            if (fptr!=NULL) fprintf(fptr,fnil);
            else              sds_printf(fnil);
            break;
        case RTT:
            if (fptr!=NULL) fprintf(fptr,ft);
            else              sds_printf(ft);
            break;
        case LSP_RTSUBR: case LSP_RTFILE:
            fcp1=(rbarg->restype==LSP_RTSUBR) ? fsubr : ffile;
            if (fptr!=NULL) fprintf(fptr,fcp1,rbarg->resval.rstring);
            else              sds_printf(fcp1,rbarg->resval.rstring);
            break;
        case LSP_RTXSUB:
            /* Find the atom by fn ptr to get the ID number */
            /* (/// THIS WILL CHANGE WHEN WE REMOVE IDs FROM THE */
            /* ATOMS LIST): */
            fi1=((tap1=lsp_findatom(rbarg->resval.rstring,1))==NULL) ?
                -9999 /* Can't find??? */ : tap1->id;
            if (fptr!=NULL) fprintf(fptr,fxsub,fi1,rbarg->resval.rstring);
            else              sds_printf(fxsub,fi1,rbarg->resval.rstring);
            break;
        case RTSHORT:
            if (fptr!=NULL) fprintf(fptr,fshort,rbarg->resval.rint);
            else              sds_printf(fshort,rbarg->resval.rint);
            break;
        case RTLB:

            /* If it's (), print nil: */
            if (rbarg->rbnext!=NULL && rbarg->rbnext->restype==RTLE) {
                if (fptr!=NULL) fprintf(fptr,fnil);
                else              sds_printf(fnil);

                break;
            }

            /* Start with '(': */
            fcp1="("/*DNT*/;
            if (fptr!=NULL) fprintf(fptr,fcp1);
            else              sds_printf(fcp1);

            /* Do the contents by calling this function recursively: */
            for (tp1=rbarg->rbnext; tp1!=NULL && tp1->restype!=RTLE; tp1=tp2) {

				if (sds_usrbrk()) { 
					msgn=20; goto out;
				}

                if ((msgn=lsp_prtrecur(tp1,fptr,mode,1))>-1) goto out;

                /* If it's not the last arg in the list, add a space: */
                if ((tp2=lsp_nextarg(tp1))!=NULL && tp2->restype!=RTLE) {
                    if (fptr!=NULL) fprintf(fptr,spstr);
                    else              sds_printf(spstr);
                }
            }

            /* End with ')': */
			// Modified CyberAge PP 08/22/2000 [
			// Fix for bug no. 2488
            if(!recur)
				fcp1=") "/*DNT*/;
			else
 				fcp1=")"/*DNT*/;
			// Modified CyberAge PP 08/22/2000 ]

           if (fptr!=NULL) fprintf(fptr,fcp1);
            else              sds_printf(fcp1);

            break;
        case LSP_RTSYMB:
            /* Okay, now, don't print the mystery symbol!: */
            if (rbarg->resval.rstring[0]==lsp_mysterysym &&
                rbarg->resval.rstring[1]==0) break;
            if (fptr!=NULL) fprintf(fptr,fstr,rbarg->resval.rstring);
            else              sds_printf(fstr,rbarg->resval.rstring);
            break;
        case RTDOTE:
            fcp1="."/*DNT*/;
            if (fptr!=NULL) fprintf(fptr,fcp1);
            else              sds_printf(fcp1);
            break;
        case RTENAME: case RTPICKS:
            fi1=0;
            if (fptr!=NULL) fprintf(fptr,(rbarg->restype==RTPICKS) ?
                fpicks : fename,rbarg->resval.rlname[fi1]);
            else              sds_printf((rbarg->restype==RTPICKS) ?
                fpicks : fename,rbarg->resval.rlname[fi1]);
            break;
        case 1004: case RTBINARY:
            if (fptr != NULL) fprintf(fptr, fbin, rbarg->restype);
            else              sds_printf(fbin, rbarg->restype);
            break;
    }

// Removed This extra print was messing up the command line, I don't think we need it.
//    if (!recur) {  /* Initial call */
//
//        /* For print, send the final space: */
//        if (mode==2) {
//            if (fptr!=NULL) fprintf(fptr,spstr); else sds_printf(spstr);
//        }
//
//    }

out:
    if (!recur && ststr!=NULL) { IC_FREE(ststr); ststr=NULL; ststrsz=0L; }
    return msgn;
}
#endif


