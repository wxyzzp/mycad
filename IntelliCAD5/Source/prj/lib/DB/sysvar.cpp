/* C:\ICAD\PRJ\LIB\DB\SYSVAR.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "sysvar_search_defs.h"
#include "sysvar_search.h"
#include <sys\timeb.h>

// this stuff adapted from header.cpp.  Header.cpp uses a much wider
// variety of data types, and can be used as an example if others, currently
// unused in sysvar.cpp, need to be added.

//compare functions for bsearch of the various system variable types
// sleazy method -- just use one of the strus 'cuz we know they all start with char *
int sysvarsearchcompare(const void *a,const void *b)
	{
	struct sysvarshortsearchstru *a1,*b1;
	a1=(struct sysvarshortsearchstru *)a;
	b1=(struct sysvarshortsearchstru *)b;
	return strcmp(a1->name,b1->name);
	}


// ctors/dtors for search classes
sysvarshortsearch::sysvarshortsearch(void)
	{
	data=sysvarshortsearchdata;
	numentries = sizeof(sysvarshortsearchdata)   / sizeof(sysvarshortsearchdata[0]);
	}


sysvarpointsearch::sysvarpointsearch(void)
	{
	data=sysvarpointsearchdata;
	numentries = sizeof(sysvarpointsearchdata) / sizeof(sysvarpointsearchdata[0]);
	}

sysvarstringsearch::sysvarstringsearch(void)
	{
	data=sysvarstringsearchdata;
	numentries = sizeof(sysvarstringsearchdata)  / sizeof(sysvarstringsearchdata[0]);
	}


sysvarshortsearch::~sysvarshortsearch(void) {}
sysvarpointsearch::~sysvarpointsearch(void) {}
sysvarstringsearch::~sysvarstringsearch(void) {}

// record finds for search classes
struct sysvarshortsearchstru *sysvarshortsearch::FindRec(char *name)
	{
	struct sysvarshortsearchstru shortkey;

	shortkey.name=name;
	return ((struct sysvarshortsearchstru *)
		bsearch(&shortkey,data,numentries,sizeof(sysvarshortsearch),sysvarsearchcompare));
	}

struct sysvarpointsearchstru *sysvarpointsearch::FindRec(char *name)
	{
	struct sysvarpointsearchstru pointkey;
	pointkey.name=name;
	return ((struct sysvarpointsearchstru *)
		bsearch(&pointkey,data,numentries,sizeof(sysvarpointsearch),sysvarsearchcompare));
	}

struct sysvarstringsearchstru *sysvarstringsearch::FindRec(char *name)
	{
	struct sysvarstringsearchstru stringkey;
	stringkey.name=name;
	return ((struct sysvarstringsearchstru *)
		bsearch(&stringkey,data,numentries,sizeof(sysvarstringsearch),sysvarsearchcompare));
	}


// the search classes
sysvarshortsearch svshortsearch;
sysvarpointsearch svpointsearch;
sysvarstringsearch svstringsearch;


extern bool	   (*db_serialNumber)( LPTSTR number, DWORD size);

// Getvar -- Searches the various DWG sysvar variable structures and retrieves the requested value.
// Translates values stored as handles, hips, or shtstrs (shorts that are passed in and out as strings) to
// strings.

short db_sysvar::Getvar(db_drawing *dp, char *name, struct resbuf *rb)
	{
	struct sysvarshortsearchstru *shortrec;
	struct sysvarpointsearchstru *pointrec;
	struct sysvarstringsearchstru *stringrec;
	char *cptr,*cptr2,ch;

// first check the special source vars
	if	(strsame(name,"ACADPREFIX"/*DNT*/))
		{
		if ((cptr=getenv("ACAD"/*DNT*/))==NULL)
			cptr=srchpath;
		rb->resval.rstring=NULL;
		db_mstrncpy(&rb->resval.rstring,cptr,-1);
		rb->restype=RTSTR;
		return RTNORM;
		}
	else if (strsame(name,"ACADVER"/*DNT*/))
		{
		db_mstrncpy(&rb->resval.rstring,"15i",-1);
		rb->restype=RTSTR;
		return RTNORM;
        }
	else if (strsame(name,"CDATE"/*DNT*/) ||strsame(name,"DATE"/*DNT*/))
		{
		short td[6];
		char *months="JanFebMarAprMayJunJulAugSepOctNovDec"/*DNT*/;

		rb->resval.rreal=0.0; 

		struct _timeb timebuffer;
		_ftime( &timebuffer );
		if ((cptr=ctime( &(timebuffer.time)))!=NULL && strlen(cptr)>23)
			{
		    td[0]=atoi(cptr+20);                          /* Year */
		    ch=cptr[7];
			cptr[7]=0;
			cptr2=strstr(months,cptr+4);
		    cptr[7]=ch;
		    td[1]=((cptr2==NULL) ? 0 : (cptr2-months)/3)+1; /* Month */
		    td[2]=atoi(cptr+8);                           /* Day */
		    td[3]=atoi(cptr+11);                          /* Hour */
		    td[4]=atoi(cptr+14);                          /* Minute */
		    td[5]=atoi(cptr+17);                          /* Second */
			}
		if (strsame(name,"CDATE"/*DNT*/))
			{
			char timebuf[IC_ACADBUF];
			/* Do string method to avoid rounding errors: */
		    sprintf(timebuf,"%d%02d%02d.%02d%02d%02d%02d"/*DNT*/,
				td[0],td[1],td[2],td[3],td[4],td[5], (timebuffer.millitm/10) );
		    rb->resval.rreal=atof(timebuf);
			}
		else 
			{  /* DATE */
			long	date[2];
			adEncodeAcadDate( td[2], td[1], td[0], td[3], td[4], td[5], date);
		// date[0] = julian date, date[1] = milliseconds of day
		    rb->resval.rreal=date[0] + date[1]/(24*60*60*1000.);
			}
		rb->restype=RTREAL;
		return RTNORM;
        }
	else if (strsame(name,"CMDNAMES"/*DNT*/))
		{
        db_mstrncpy(&rb->resval.rstring,""/*DNT*/,-1);		// NOP???
		rb->restype=RTSTR;
		return RTNORM;
        }
	else if (strsame(name,"DWGNAME"/*DNT*/))
		{
		char fnbuf[IC_ACADBUF],drive[IC_ACADBUF],path[IC_ACADBUF],fn[IC_ACADBUF],ext[IC_ACADBUF];
		if (dp!=NULL && dp->ret_pn()!=NULL)
			{
			_splitpath(dp->ret_pn(),drive,path,fn,ext);
			sprintf(fnbuf,"%s%s"/*DNT*/,fn,ext);
			db_mstrncpy(&rb->resval.rstring,fnbuf,-1);
			}
		else
			db_mstrncpy(&rb->resval.rstring,""/*DNT*/,-1);

		rb->restype=RTSTR;
		return RTNORM;
		}
	else if (strsame(name,"DWGPREFIX"/*DNT*/))
		{
		char fnbuf[IC_ACADBUF],drive[IC_ACADBUF],path[IC_ACADBUF],fn[IC_ACADBUF],ext[IC_ACADBUF];
		if (dp!=NULL && dp->ret_pn()!=NULL)
			{
			_splitpath(dp->ret_pn(),drive,path,fn,ext);
			sprintf(fnbuf,"%s%s"/*DNT*/,drive,path);
			db_mstrncpy(&rb->resval.rstring,fnbuf,-1);
			}
		else
			db_mstrncpy(&rb->resval.rstring,""/*DNT*/,-1);
		rb->restype=RTSTR;
		return RTNORM;
		}
	else if (strsame(name,"LOCALE"/*DNT*/))
		{
		db_mstrncpy(&rb->resval.rstring,"en"/*DNT*/,-1);
		rb->restype=RTSTR;
		return RTNORM;
        }
	else if (strsame(name,"NODENAME"/*DNT*/))
		{
		db_mstrncpy(&rb->resval.rstring,""/*DNT*/,-1);    // NOP??? is this even used?
		rb->restype=RTSTR;
		return RTNORM;
        } 
	else if (strsame(name,"PLATFORM"/*DNT*/))
		{
		char strval[IC_ACADBUF];
        char *vers="Microsoft Windows %s Version %i.%i"/*DNT*/;
		OSVERSIONINFO vi;
		vi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
		GetVersionEx(&vi);
		switch(vi.dwPlatformId)
			{
            case VER_PLATFORM_WIN32s:        
				sprintf(strval,vers,"3.1"/*DNT*/,vi.dwMajorVersion,vi.dwMinorVersion);
				break;
            case VER_PLATFORM_WIN32_WINDOWS: 
				sprintf(strval,vers,"95"/*DNT*/,vi.dwMajorVersion,vi.dwMinorVersion);
				break;
            case VER_PLATFORM_WIN32_NT:      
				sprintf(strval,vers,"NT"/*DNT*/,vi.dwMajorVersion,vi.dwMinorVersion);
				break;
            default:                         
				sprintf(strval,vers,""/*DNT*/,vi.dwMajorVersion,vi.dwMinorVersion);
				break;
			}
		db_mstrncpy(&rb->resval.rstring,strval/*DNT*/,-1);
		rb->restype=RTSTR;
		return RTNORM;
        }
	else if (strsame(name,"SAVENAME"/*DNT*/))
		{
        if (dp!=NULL && dp->ret_pn()!=NULL)
			db_mstrncpy(&rb->resval.rstring,dp->ret_pn(),-1);
		else
			db_mstrncpy(&rb->resval.rstring,""/*DNT*/,-1);
		rb->restype=RTSTR;
		return RTNORM;
		}
	else if (strsame(name,"SYSCODEPAGE"/*DNT*/)) 
		{
		TCHAR	codePage[20];
		TCHAR	codePageName[24];
		int size = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, codePage, sizeof(codePage));
        sprintf(codePageName,"%s%s","ANSI_"/*DNT*/,codePage);
		db_mstrncpy(&rb->resval.rstring,codePageName,-1);
		rb->restype=RTSTR;
		return RTNORM;
		}
	else if (strsame(name,"_LINFO"/*DNT*/))
		{
		db_mstrncpy(&rb->resval.rstring,""/*DNT*/,-1);  // NOP??  used?
		rb->restype=RTSTR;
		return RTNORM;
        }
	else if (strsame(name,"_PKSER"/*DNT*/))
		{
		char serbuf[IC_ACADBUF];
		if ( db_serialNumber )
			{
			(*db_serialNumber)( serbuf,IC_ACADBUF);
			db_mstrncpy(&rb->resval.rstring,serbuf,-1);
			}
		else
			db_mstrncpy(&rb->resval.rstring,"",-1);
		rb->restype=RTSTR;
		return RTNORM;
        }
	else if (strsame(name,"_SERVER"/*DNT*/))
		{
		rb->resval.rint=-1;  // NOP?? used?
		rb->restype=RTSHORT;
		return RTNORM;
		}
	else if (strsame(name,"CMDACTIVE"/*DNT*/))
		{
// need to see how to handle this
		// This is a very special variable that is dependent on global
		// state of the IntelliCAD engine.  As such it is set in SDS_getvar() at a much higher
		// level and should not be encountered down here
		//
		ASSERT( FALSE );
		}

	shortrec=svshortsearch.FindRec(name);
	if (shortrec!=NULL)
		{
		rb->resval.rint=(this->*(shortrec->retvarfn))();	// and call the retvar member function (bizarre syntax, huh?)
		rb->restype=RTSHORT;							// set up the resbuf
		return RTNORM;									// and return
		}

	pointrec=svpointsearch.FindRec(name);
	if (pointrec!=NULL)
		{
		(this->*(pointrec->getvarfn))(rb->resval.rpoint);	// and call the retvar member function
		rb->resval.rpoint[2]=0.0;
		rb->restype=RTPOINT;							// set up the resbuf
		return RTNORM;									// and return
		}

	stringrec=svstringsearch.FindRec(name);
	if (stringrec!=NULL)
		{
		char *thestr=(this->*(stringrec->retvarfn))();			// and call the retvar member function
		if ((rb->resval.rstring= new char [strlen(thestr)+1])==NULL)
			return RTERROR;
		strcpy(rb->resval.rstring,thestr);
		rb->restype=RTSTR;								// set up the resbuf
		return RTNORM;									// and return
		}

	return RTERROR;			// not found
	}


// Setvar -- Searches the various DWG sysvar variable structures and stores the given value.
// Translates values to be stored as handles, hips, or shtstrs (shorts that are passed in and out as strings) from
// strings to the appropriate type.  Performs range checks as appropriate.

short db_sysvar::Setvar(db_drawing *dp, char *name,struct resbuf *rb)
	{
	struct sysvarshortsearchstru *shortrec;
	struct sysvarpointsearchstru *pointrec;
	struct sysvarstringsearchstru *stringrec;

// check special source items first (only one as of now)
	if (strsame(name,"DWGNAME"/*DNT*/))
		{
		if (rb->restype!=RTSTR)
			return RTERROR;
		if (dp==NULL)
			return RTERROR;
		dp->SetPathName(rb->resval.rstring);
		}
	
// end of special source items
	
	shortrec=svshortsearch.FindRec(name);
	if (shortrec!=NULL)
		{
		if (rb->restype!=RTSHORT)
			return RTERROR;
		if (shortrec->readonly)
			return RTERROR;
		if (shortrec->hasmin && rb->resval.rint<shortrec->min)
			return RTERROR;
		if (shortrec->hasmax && rb->resval.rint>shortrec->max)
			return RTERROR;
		(this->*(shortrec->setvarfn))(rb->resval.rint);	// call the setvar member function (bizarre syntax, huh?)
		return RTNORM;									// and return
		}

	pointrec=svpointsearch.FindRec(name);
	if (pointrec!=NULL)
		{
		if (rb->restype!=RTPOINT)
			return RTERROR;
		if (pointrec->readonly)
			return RTERROR;
		(this->*(pointrec->setvarfn))(rb->resval.rpoint);	// call the setvar member function
		return RTNORM;										// and return
		}

	stringrec=svstringsearch.FindRec(name);
	if (stringrec!=NULL)
		{
		if (rb->restype!=RTSTR)
			return RTERROR;
		if (stringrec->readonly)
			return RTERROR;
		(this->*(stringrec->setvarfn))(rb->resval.rstring);	// call the setvar member function
		return RTNORM;										// and return
		}

	return RTERROR;			// not found
	}


short db_sysvar::RetAperture(void)
	{
	return aperture;
	}

short db_sysvar::SetAperture(short a)
	{
	aperture=a;
	return RTNORM;
	}

short db_sysvar::RetAuditctl(void)
	{
	return auditctl;
	}

short db_sysvar::SetAuditctl(short a)
	{
	auditctl=a;
	return RTNORM;
	}

short db_sysvar::RetAutomenuload(void)
	{
	return automenuload;
	}

short db_sysvar::SetAutomenuload(short a)
	{
	automenuload=a;
	return RTNORM;
	}

char * db_sysvar::RetBasefile(void)
	{
	return basefile;
	}

short db_sysvar::SetBasefile(char *a)
	{
	db_astrncpy(&basefile,a,-1);
	return RTNORM;
	}

short db_sysvar::RetBkgcolor(void)
	{
	return bkgcolor;
	}

short db_sysvar::SetBkgcolor(short a)
	{
	bkgcolor=a;
	return RTNORM;
	}

short db_sysvar::RetBkgredraw(void)
	{
	return bkgredraw;
	}

short db_sysvar::SetBkgredraw(short a)
	{
	bkgredraw=a;
	return RTNORM;
	}

short db_sysvar::RetCmddia(void)
	{
	return cmddia;
	}

short db_sysvar::SetCmddia(short a)
	{
	cmddia=a;
	return RTNORM;
	}

char * db_sysvar::RetCmdlntext(void)
	{
	return cmdlntext;
	}

short db_sysvar::SetCmdlntext(char *a)
	{
	db_astrncpy(&cmdlntext,a,-1);
	return RTNORM;
	}

short db_sysvar::RetColorx(void)
	{
	return colorx;
	}
short db_sysvar::SetColorx(short a)
	{
	colorx=a;
	return RTNORM;
	}

short db_sysvar::RetColory(void)
	{
	return colory;
	}

short db_sysvar::SetColory(short a)
	{
	colory=a;
	return RTNORM;
	}

short db_sysvar::RetColorz(void)
	{
	return colorz;
	}

short db_sysvar::SetColorz(short a)
	{
	colorz=a;
	return RTNORM;
	}

char * db_sysvar::RetDctcust(void)
	{
	return dctcust;
	}

short db_sysvar::SetDctcust(char *a)
	{
	db_astrncpy(&dctcust,a,-1);
	return RTNORM;
	}

char * db_sysvar::RetDctmain(void)
	{
	return dctmain;
	}

short db_sysvar::SetDctmain(char *a)
	{
	db_astrncpy(&dctmain,a,-1);
	return RTNORM;
	}

short db_sysvar::RetDimSS()
{
	return dimss;
}

short db_sysvar::SetDimSS( short dimSS )
{
	dimss = dimSS;
	return RTNORM;
}

short db_sysvar::RetDimSSPos()
{
	return dimsspos;
}

short db_sysvar::SetDimSSPos( short dimSSPos )
{
	dimsspos = dimSSPos;
	return RTNOTM;
}

short db_sysvar::RetDragopen(void)
	{
	return dragopen;
	}

short db_sysvar::SetDragopen(short a)
	{
	dragopen=a;
	return RTNORM;
	}

short db_sysvar::RetDragp1(void)
	{
	return dragp1;
	}

short db_sysvar::SetDragp1(short a)
	{
	dragp1=a;
	return RTNORM;
	}

short db_sysvar::RetDragp2(void)
	{
	return dragp2;
	}

short db_sysvar::SetDragp2(short a)
	{
	dragp2=a;
	return RTNORM;
	}

short db_sysvar::RetErrorno(void)
	{
	return errorno;
	}

short db_sysvar::SetErrorno(short a)
	{
	errorno=a;
	return RTNORM;
	}

short db_sysvar::RetExpert(void)
	{
	return expert;
	}

short db_sysvar::SetExpert(short a)
	{
	expert=a;
	return RTNORM;
	}

short db_sysvar::RetExplevel(void)
	{
	return explevel;
	}

short db_sysvar::SetExplevel(short a)
	{
	explevel=a;
	return RTNORM;
	}

char * db_sysvar::RetExplstblk(void)
	{
	return explstblk;
	}

short db_sysvar::SetExplstblk(char *a)
	{
	db_astrncpy(&explstblk,a,-1);
	return RTNORM;
	}

char * db_sysvar::RetExplstds(void)
	{
	return explstds;
	}

short db_sysvar::SetExplstds(char *a)
	{
	db_astrncpy(&explstds,a,-1);
	return RTNORM;
	}

char * db_sysvar::RetExplstlay(void)
	{
	return explstlay;
	}

short db_sysvar::SetExplstlay(char *a)
	{
	db_astrncpy(&explstlay,a,-1);
	return RTNORM;
	}

char * db_sysvar::RetExplstlt(void)
	{
	return explstlt;
	}

short db_sysvar::SetExplstlt(char *a)
	{
	db_astrncpy(&explstlt,a,-1);
	return RTNORM;
	}

char * db_sysvar::RetExplstts(void)
	{
	return explstts;
	}

short db_sysvar::SetExplstts(char *a)
	{
	db_astrncpy(&explstts,a,-1);
	return RTNORM;
	}

char * db_sysvar::RetExplstucs(void)
	{
	return explstucs;
	}

short db_sysvar::SetExplstucs(char *a)
	{
	db_astrncpy(&explstucs,a,-1);
	return RTNORM;
	}

char * db_sysvar::RetExplstvw(void)
	{
	return explstvw;
	}

short db_sysvar::SetExplstvw(char *a)
	{
	db_astrncpy(&explstvw,a,-1);
	return RTNORM;
	}

short db_sysvar::RetFflimit(void)
	{
	return fflimit;
	}

short db_sysvar::SetFflimit(short a)
	{
	fflimit=a;
	return RTNORM;
	}

short db_sysvar::RetFiledia(void)
	{
	return filedia;
	}

short db_sysvar::SetFiledia(short a)
	{
	filedia=a;
	return RTNORM;
	}

char * db_sysvar::RetFontalt(void)
	{
	return fontalt;
	}

short db_sysvar::SetFontalt(char *a)
	{
	db_astrncpy(&fontalt,a,-1);
	return RTNORM;
	}

char * db_sysvar::RetFontmap(void)
	{
	return fontmap;
	}

short db_sysvar::SetFontmap(char *a)
	{
	db_astrncpy(&fontmap,a,-1);
	return RTNORM;
	}

short db_sysvar::RetGlobcheck(void)
	{
	return globcheck;
	}

short db_sysvar::SetGlobcheck(short a)
	{
	globcheck=a;
	return RTNORM;
	}

short db_sysvar::RetGripblock(void)
	{
	return gripblock;
	}

short db_sysvar::SetGripblock(short a)
	{
	gripblock=a;
	return RTNORM;
	}

short db_sysvar::RetGripcolor(void)
	{
	return gripcolor;
	}

short db_sysvar::SetGripcolor(short a)
	{
	gripcolor=a;
	return RTNORM;
	}

short db_sysvar::RetGriphot(void)
	{
	return griphot;
	}

short db_sysvar::SetGriphot(short a)
	{
	griphot=a;
	return RTNORM;
	}

short db_sysvar::RetGrips(void)
	{
	return grips;
	}

short db_sysvar::SetGrips(short a)
	{
	grips=a;
	return RTNORM;
	}

short db_sysvar::RetGripsize(void)
	{
	return gripsize;
	}

short db_sysvar::SetGripsize(short a)
	{
	gripsize=a;
	return RTNORM;
	}

short db_sysvar::RetHandudcopy(void)
	{
	return handudcopy;
	}

short db_sysvar::SetHandudcopy(short a)
	{
	handudcopy=a;
	return RTNORM;
	}

short db_sysvar::RetIsavepercent(void)
	{
	return isavepercent;
	}

short db_sysvar::SetIsavepercent(short a)
	{
	isavepercent=a;
	return RTNORM;
	}

char * db_sysvar::RetLastprompt(void)
	{
	return lastprompt;
	}

short db_sysvar::SetLastprompt(char *a)
	{
	db_astrncpy(&lastprompt,a,-1);
	return RTNORM;
	}

short db_sysvar::RetLogfilemode(void)
	{
	return logfilemode;
	}

short db_sysvar::SetLogfilemode(short a)
	{
	logfilemode=a;
	return RTNORM;
	}

char * db_sysvar::RetLogfilename(void)
	{
	return logfilename;
	}

short db_sysvar::SetLogfilename(char *a)
	{
	db_astrncpy(&logfilename,a,-1);
	return RTNORM;
	}

char * db_sysvar::RetLoginname(void)
	{
	return loginname;
	}

short db_sysvar::SetLoginname(char *a)
	{
	db_astrncpy(&loginname,a,-1);
	return RTNORM;
	}

short db_sysvar::RetLongfname(void)
	{
	return longfname;
	}

short db_sysvar::SetLongfname(short a)
	{
	longfname=a;
	return RTNORM;
	}

short db_sysvar::RetLwpolyout(void)
	{
	return lwpolyout;
	}

short db_sysvar::SetLwpolyout(short a)
	{
	lwpolyout=a;
	return RTNORM;
	}

short db_sysvar::RetMacrorec(void)
	{
	return macrorec;
	}
short db_sysvar::SetMacrorec(short a)
	{
	macrorec=a;
	return RTNORM;
	}

short db_sysvar::RetMacrotrace(void)
	{
	return macrotrace;
	}

short db_sysvar::SetMacrotrace(short a)
	{
	macrotrace=a;
	return RTNORM;
	}

short db_sysvar::RetMakebak(void)
	{
	return makebak;
	}

short db_sysvar::SetMakebak(short a)
	{
	makebak=a;
	return RTNORM;
	}

long db_sysvar::RetMaxobjmem(void)
	{
	return maxobjmem;
	}

short db_sysvar::SetMaxobjmem(long a)
	{
	maxobjmem=a;
	return RTNORM;
	}

short db_sysvar::RetMaxsort(void)
	{
	return maxsort;
	}

short db_sysvar::SetMaxsort(short a)
	{
	maxsort=a;
	return RTNORM;
	}

short db_sysvar::RetMbuttonpan()
{
	return mbuttonpan;
}

short db_sysvar::SetMbuttonpan(short MButtonPan)
{
	mbuttonpan = MButtonPan;
	return RTNORM;
}

short db_sysvar::RetMenuctl(void)
	{
	return menuctl;
	}

short db_sysvar::SetMenuctl(short a)
	{
	menuctl=a;
	return RTNORM;
	}

char * db_sysvar::RetModemacro(void)
	{
	return modemacro;
	}

short db_sysvar::SetModemacro(char *a)
	{
	db_astrncpy(&modemacro,a,-1);
	return RTNORM;
	}

char * db_sysvar::RetMtexted(void)
	{
	return mtexted;
	}

short db_sysvar::SetMtexted(char *a)
	{
	db_astrncpy(&mtexted,a,-1);
	return RTNORM;
	}

short db_sysvar::RetNfilelist(void)
	{
	return nfilelist;
	}

short db_sysvar::SetNfilelist(short a)
	{
	nfilelist=a;
	return RTNORM;
	}

short db_sysvar::RetPfacevmax(void)
	{
	return pfacevmax;
	}

short db_sysvar::SetPfacevmax(short a)
	{
	pfacevmax=a;
	return RTNORM;
	}

long db_sysvar::RetPhandle(void)
	{
	return phandle;
	}

short db_sysvar::SetPhandle(long a)
	{
	phandle=a;
	return RTNORM;
	}

short db_sysvar::RetPickadd(void)
	{
	return pickadd;
	}

short db_sysvar::SetPickadd(short a)
	{
	pickadd=a;
	return RTNORM;
	}

short db_sysvar::RetPickauto(void)
	{
	return pickauto;
	}

short db_sysvar::SetPickauto(short a)
	{
	pickauto=a;
	return RTNORM;
	}

short db_sysvar::RetPickbox(void)
	{
	return pickbox;
	}

short db_sysvar::SetPickbox(short a)
	{
	pickbox=a;
	return RTNORM;
	}

short db_sysvar::RetPickdrag(void)
	{
	return pickdrag;
	}

short db_sysvar::SetPickdrag(short a)
	{
	pickdrag=a;
	return RTNORM;
	}

short db_sysvar::RetPickfirst(void)
	{
	return pickfirst;
	}

short db_sysvar::SetPickfirst(short a)
	{
	pickfirst=a;
	return RTNORM;
	}

short db_sysvar::RetPlinetype(void)
	{
	return plinetype;
	}

short db_sysvar::SetPlinetype(short a)
	{
	plinetype=a;
	return RTNORM;
	}

char * db_sysvar::RetPlotid(void)
	{
	return plotid;
	}

short db_sysvar::SetPlotid(char *a)
	{
	db_astrncpy(&plotid,a,-1);
	return RTNORM;
	}

short db_sysvar::RetPlotrotmode(void)
	{
	return plotrotmode;
	}

short db_sysvar::SetPlotrotmode(short a)
	{
	plotrotmode=a;
	return RTNORM;
	}

short db_sysvar::RetPlotter(void)
	{
	return plotter;
	}

short db_sysvar::SetPlotter(short a)
	{
	plotter=a;
	return RTNORM;
	}

short db_sysvar::RetPopups(void)
	{
	return popups;
	}

short db_sysvar::SetPopups(short a)
	{
	popups=a;
	return RTNORM;
	}

short db_sysvar::RetProgbar(void)
	{
	return progbar;
	}

short db_sysvar::SetProgbar(short a)
	{
	progbar=a;
	return RTNORM;
	}

short db_sysvar::RetProjmode(void)
	{
	return projmode;
	}

short db_sysvar::SetProjmode(short a)
	{
	projmode=a;
	return RTNORM;
	}

short db_sysvar::RetPromptmenu(void)
	{
	return promptmenu;
	}

short db_sysvar::SetPromptmenu(short a)
	{
	promptmenu=a;
	return RTNORM;
	}

char * db_sysvar::RetPsprolog(void)
	{
	return psprolog;
	}

short db_sysvar::SetPsprolog(char *a)
	{
	db_astrncpy(&psprolog,a,-1);
	return RTNORM;
	}

short db_sysvar::RetPsquality(void)
	{
	return psquality;
	}

short db_sysvar::SetPsquality(short a)
	{
	psquality=a;
	return RTNORM;
	}

short db_sysvar::RetQaflags(void)
	{
	return qaflags;
	}

short db_sysvar::SetQaflags(short a)
	{
	qaflags=a;
	return RTNORM;
	}

short db_sysvar::RetRasterpreview(void)
	{
	return rasterpreview;
	}

short db_sysvar::SetRasterpreview(short a)
	{
	rasterpreview=a;
	return RTNORM;
	}

short db_sysvar::RetRe_init(void)
	{
	return re_init;
	}

short db_sysvar::SetRe_init(short a)
	{
	re_init=a;
	return RTNORM;
	}

char * db_sysvar::RetSavefile(void)
	{
	return savefile;
	}

short db_sysvar::SetSavefile(char *a)
	{
	db_astrncpy(&savefile,a,-1);
	return RTNORM;
	}

short db_sysvar::RetSavetime(void)
	{
	return savetime;
	}

short db_sysvar::SetSavetime(short a)
	{
	savetime=a;
	return RTNORM;
	}

short db_sysvar::RetScreenboxes(void)
	{
	return screenboxes;
	}

short db_sysvar::SetScreenboxes(short a)
	{
	screenboxes=a;
	return RTNORM;
	}

short db_sysvar::RetScreenmode(void)
	{
	return screenmode;
	}

short db_sysvar::SetScreenmode(short a)
	{
	screenmode=a;
	return RTNORM;
	}

void db_sysvar::GetScreenpos(sds_point a)
	{
	a[0]=screenpos[0];
	a[1]=screenpos[1];
	a[2]=0.0;
	}

short db_sysvar::SetScreenpos(sds_point a)
	{
	screenpos[0]=a[0];
	screenpos[1]=a[1];
	screenpos[2]=0.0;
	return RTNORM;
	}


void db_sysvar::GetScreensize(sds_point a)
	{
	a[0]=screensize[0];
	a[1]=screensize[1];
	a[2]=0.0;
	}

short db_sysvar::SetScreensize(sds_point a)
	{
	screensize[0]=a[0];
	screensize[1]=a[1];
	screensize[2]=0.0;
	return RTNORM;
	}

short db_sysvar::RetScrlhist(void)
	{
	return scrlhist;
	}

short db_sysvar::SetScrlhist(short a)
	{
	scrlhist=a;
	return RTNORM;
	}

short db_sysvar::RetSortents(void)
	{
	return sortents;
	}

short db_sysvar::SetSortents(short a)
	{
	sortents=a;
	return RTNORM;
	}

char * db_sysvar::RetSrchpath(void)
	{
	return srchpath;
	}

short db_sysvar::SetSrchpath(char *a)
	{
	db_astrncpy(&srchpath,a,-1);
	return RTNORM;
	}

short db_sysvar::RetTabmode(void)
	{
	return tabmode;
	}

short db_sysvar::SetTabmode(short a)
	{
	tabmode=a;
	return RTNORM;
	}

short db_sysvar::RetTbcolor(void)
	{
	return tbcolor;
	}

short db_sysvar::SetTbcolor(short a)
	{
	tbcolor=a;
	return RTNORM;
	}

short db_sysvar::RetTbsize(void)
	{
	return tbsize;
	}

short db_sysvar::SetTbsize(short a)
	{
	tbsize=a;
	return RTNORM;
	}

char * db_sysvar::RetTempprefix(void)
	{
	return tempprefix;
	}

short db_sysvar::SetTempprefix(char *a)
	{
	db_astrncpy(&tempprefix,a,-1);
	return RTNORM;
	}

short db_sysvar::RetTexteval(void)
	{
	return texteval;
	}

short db_sysvar::SetTexteval(short a)
	{
	texteval=a;
	return RTNORM;
	}

short db_sysvar::RetTextfill(void)
	{
	return textfill;
	}

short db_sysvar::SetTextfill(short a)
	{
	textfill=a;
	return RTNORM;
	}

short db_sysvar::RetTipofday(void)
	{
	return tipofday;
	}

short db_sysvar::SetTipofday(short a)
	{
	tipofday=a;
	return RTNORM;
	}

short db_sysvar::RetTooltips(void)
	{
	return tooltips;
	}

short db_sysvar::SetTooltips(short a)
	{
	tooltips=a;
	return RTNORM;
	}

long db_sysvar::RetTreemax(void)
	{
	return treemax;
	}

short db_sysvar::SetTreemax(long a)
	{
	treemax=a;
	return RTNORM;
	}


short db_sysvar::RetUcsiconpos(void)
	{
	return ucsiconpos;
	}

short db_sysvar::SetUcsiconpos(short a)
	{
	ucsiconpos=a;
	return RTNORM;
	}

short db_sysvar::RetWndbtexp(void)
	{
	return wndbtexp;
	}

short db_sysvar::SetWndbtexp(short a)
	{
	wndbtexp=a;
	return RTNORM;
	}

short db_sysvar::RetWndlcmd(void)
	{
	return wndlcmd;
	}

short db_sysvar::SetWndlcmd(short a)
	{
	wndlcmd=a;
	return RTNORM;
	}

short db_sysvar::RetWndlmain(void)
	{
	return wndlmain;
	}

short db_sysvar::SetWndlmain(short a)
	{
	wndlmain=a;
	return RTNORM;
	}

short db_sysvar::RetWndlmdi(void)
	{
	return wndlmdi;
	}

short db_sysvar::SetWndlmdi(short a)
	{
	wndlmdi=a;
	return RTNORM;
	}

short db_sysvar::RetWndlscrl(void)
	{
	return wndlscrl;
	}

short db_sysvar::SetWndlscrl(short a)
	{
	wndlscrl=a;
	return RTNORM;
	}

short db_sysvar::RetWndlstat(void)
	{
	return wndlstat;
	}

short db_sysvar::SetWndlstat(short a)
	{
	wndlstat=a;
	return RTNORM;
	}

short db_sysvar::RetWndltexp(void)
	{
	return wndltexp;
	}

short db_sysvar::SetWndltexp(short a)
	{
	wndltexp=a;
	return RTNORM;
	}

short db_sysvar::RetWndltext(void)
	{
	return wndltext;
	}

short db_sysvar::SetWndltext(short a)
	{
	wndltext=a;
	return RTNORM;
	}

void db_sysvar::GetWndpcmd(sds_point a)
	{
	a[0]=wndpcmd[0];
	a[1]=wndpcmd[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndpcmd(sds_point a)
	{
	wndpcmd[0]=a[0];
	wndpcmd[1]=a[1];
	wndpcmd[2]=0.0;
	return RTNORM;
	}

void db_sysvar::GetWndpform(sds_point a)
	{
	a[0]=wndpform[0];
	a[1]=wndpform[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndpform(sds_point a)
	{
	wndpform[0]=a[0];
	wndpform[1]=a[1];
	wndpform[2]=0.0;
	return RTNORM;
	}

void db_sysvar::GetWndpmain(sds_point a)
	{
	a[0]=wndpmain[0];
	a[1]=wndpmain[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndpmain(sds_point a)
	{
	wndpmain[0]=a[0];
	wndpmain[1]=a[1];
	wndpmain[2]=0.0;
	return RTNORM;
	}

void db_sysvar::GetWndppmenu(sds_point a)
	{
	a[0]=wndppmenu[0];
	a[1]=wndppmenu[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndppmenu(sds_point a)
	{
	wndppmenu[0]=a[0];
	wndppmenu[1]=a[1];
	wndppmenu[2]=0.0;
	return RTNORM;
	}

void db_sysvar::GetWndptexp(sds_point a)
	{
	a[0]=wndptexp[0];
	a[1]=wndptexp[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndptexp(sds_point a)
	{
	wndptexp[0]=a[0];
	wndptexp[1]=a[1];
	wndptexp[2]=0.0;
	return RTNORM;
	}

void db_sysvar::GetWndptext(sds_point a)
	{
	a[0]=wndptext[0];
	a[1]=wndptext[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndptext(sds_point a)
	{
	wndptext[0]=a[0];
	wndptext[1]=a[1];
	wndptext[2]=0.0;
	return RTNORM;
	}

void db_sysvar::GetWndscmd(sds_point a)
	{
	a[0]=wndscmd[0];
	a[1]=wndscmd[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndscmd(sds_point a)
	{
	wndscmd[0]=a[0];
	wndscmd[1]=a[1];
	wndscmd[2]=0.0;
	return RTNORM;
	}

void db_sysvar::GetWndsform(sds_point a)
	{
	a[0]=wndsform[0];
	a[1]=wndsform[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndsform(sds_point a)
	{
	wndsform[0]=a[0];
	wndsform[1]=a[1];
	wndsform[2]=0.0;
	return RTNORM;
	}

void db_sysvar::GetWndsmain(sds_point a)
	{
	a[0]=wndsmain[0];
	a[1]=wndsmain[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndsmain(sds_point a)
	{
	wndsmain[0]=a[0];
	wndsmain[1]=a[1];
	wndsmain[2]=0.0;
	return RTNORM;
	}

void db_sysvar::GetWndstexp(sds_point a)
	{
	a[0]=wndstexp[0];
	a[1]=wndstexp[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndstexp(sds_point a)
	{
	wndstexp[0]=a[0];
	wndstexp[1]=a[1];
	wndstexp[2]=0.0;
	return RTNORM;
	}

void db_sysvar::GetWndstext(sds_point a)
	{
	a[0]=wndstext[0];
	a[1]=wndstext[1];
	a[2]=0.0;
	}

short db_sysvar::SetWndstext(sds_point a)
	{
	wndstext[0]=a[0];
	wndstext[1]=a[1];
	wndstext[2]=0.0;
	return RTNORM;
	}

short db_sysvar::RetXloadctl(void)
	{
	return xloadctl;
	}

short db_sysvar::SetXloadctl(short a)
	{
	xloadctl=a;
	return RTNORM;
	}

char * db_sysvar::RetXloadpath(void)
	{
	return xloadpath;
	}

short db_sysvar::SetXloadpath(char *a)
	{
	db_astrncpy(&xloadpath,a,-1);
	return RTNORM;
	}

short db_sysvar::RetXrefctl(void)
	{
	return xrefctl;
	}

short db_sysvar::SetXrefctl(short a)
	{
	xrefctl=a;
	return RTNORM;
	}

short db_sysvar::RetZoomfactor()
{
	return zoomfactor;
}

short db_sysvar::SetZoomfactor(short ZoomFactor)
{
	zoomfactor = ZoomFactor;
	return RTNOTM;
}

char * db_sysvar::Ret_vernum(void)
	{
	return _vernum;
	}

short db_sysvar::Set_vernum(char *a)
	{
	db_astrncpy(&_vernum,a,-1);
	return RTNORM;
	}

