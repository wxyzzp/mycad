/* D:\ICAD\PRJ.MAIN\LIB\DB\OLDVARS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */
#include "db.h"/*DNT*/
#include "objects.h"

#include "oldvars.h"

#include <sys/timeb.h>
#include <assert.h>
#include <tchar.h>

/*
**							  TIME LOGIC
**
**
**	The TD system variables are a little tricky -- especially
**	those concerning the user-timer.  Here's the logic:
**
**	TDCREATE:
**
**	  Set in the buffer via setvar when the drawing is first
**	  created.	Maintained during open/save thereafter.
**	  Getvar just returns whatever is in the buffer.
**
**	TDUPDATE:
**
**	  Set in the buffer via setvar when the drawing is first
**	  created -- and whenever it is saved.
**	  Getvar just returns whatever is in the buffer.
**
**	TDINDWG:
**
**	  Set to 0.0 in the new header when the drawing is created.
**	  When the drawing is saved, the getvar value is written to the
**	  drawing file.  Getvar returns the value in the buffer
**	  plus the elapsed time in the current session.
**
**	TDUSRTIMER:
**
**	  Set to 0.0 in the new header when the drawing is created.
**	  When the drawing is saved, the getvar value is written to the
**	  drawing file.  Getvar returns the following:
**
**		USRTIMER OFF:
**
**		  The value in the buffer.
**
**		USRTIMER ON:
**
**		  The value in the buffer plus the elapsed time since
**		  USRTIMER was most recently toggled from OFF to ON.
**
**	  Whenever setvar toggles USRTIMER from OFF to ON, in addition to
**	  setting USRTIMER to 1, it saves the time this occurred in the
**	  db_drawing object.
**
**	  Whenever setvar toggles USRTIMER from ON to OFF, in addition to
**	  setting USRTIMER to 0, it adds to TDUSRTIMER in the buffer the
**	  time that has elapsed since USRTIMER was most recently toggled
**	  from OFF to ON.
*/


#if defined(DEBUG)
	/*
	**	A pointer to a helper function to give info about errors.
	**	Value of wherefrom:
	**		0 : getvar
	**		1 : setvar
	*/
	void (*db_errormessfnp)(char *mess, int wherefrom);

	/*
	**	Getvar/setvar error messages:
	*/
	char *db_getsetvarmess[]={
		"Resbuf is NULL",						 /*  0 */
		"Variable not found",					 /*  1 */
		"Bad type in database", 				 /*  2 */
		"Case not found for \"special source\"", /*  3 */
		"Bad buffer idx in database",			 /*  4 */
		"Buffer is NULL",						 /*  5 */
		"Bad position in buffer",				 /*  6 */
		"Handle conversion failed", 			 /*  7 */
		"Case not found for \"special format\"", /*  8 */
		"Type mismatch",						 /*  9 */
		"True read-only",						 /* 10 */
		"Value out of range",					 /* 11 */
		"Drawing pointer is NULL",				 /* 12 */
		"Table record not found",				 /* 13 */
		"Zero vector not allowed",				 /* 14 */
		"String too long",						 /* 15 */
		"Case not found for \"special case\""	 /* 16 */
	};

#endif

#define SETVAR_NO_ERROR						   -1
#define SETVAR_RESBUF_IS_NULL					0
#define SETVAR_VARIABLE_NOT_FOUND				1
#define SETVAR_BAD_TYPE_IN_DATABASE				2
#define SETVAR_SPECIAL_SOURCE_CASE_NOT_FOUND	3
#define SETVAR_BAD_BUFFER_IDX_IN_DATABASE		4
#define SETVAR_BUFFER_IS_NULL					5
#define SETVAR_BAD_POSITION_IN_BUFFER			6
#define SETVAR_HANDLE_CONVERSION_FAILED			7
#define SETVAR_SPECIAL_FORMAT_CASE_NOT_FOUND	8
#define SETVAR_TYPE_MISMATCH					9
#define SETVAR_TRUE_READ_ONLY					10
#define SETVAR_VALUE_OUT_OF_RANGE				11
#define SETVAR_DRAWING_POINTER_IS_NULL			12
#define SETVAR_TABLE_RECORD_NOT_FOUND			13
#define SETVAR_ZERO_VECTOR_NOT_ALLOWED			14
#define SETVAR_STRING_TOO_LONG					15
#define SETVAR_SPECIAL_CASE_CASE_NOT_FOUND		16

db_dictionaryvar *GetVarFromObjectList(const char *vname,db_drawing *dp)
{
	db_handitem *stobj,*endobj;
	db_dictionaryrec *vbldicrec,*dicvarobjrec;
	db_dictionary *vbldic;

	dp->get_objllends(&stobj,&endobj);
// we assume that the first object is the main dictionary item
// find the dicrec that points to the variable dictionary
	vbldicrec=((db_dictionary *)stobj)->findrec("ACDBVARIABLEDICTIONARY"/*DNT*/,0,dp);
	if (vbldicrec==NULL) return(NULL);
// get the dictionary to which that dicrec points
	vbldic=(db_dictionary *)(vbldicrec->hiref->ret_hip(dp));
	if (vbldic==NULL) return(NULL);
// get the dicrec which points to the dictionaryvar object for this variable
	dicvarobjrec=vbldic->findrec(vname,0,dp);
	if (dicvarobjrec==NULL) return(NULL);
// return a pointer to the dictionaryvar object, NULL if not found
	return (db_dictionaryvar *)(dicvarobjrec->hiref->ret_hip(dp));
}


int GetSysvarFromObjects(db_sysvarlink *found,db_drawing *dp,struct resbuf *res)
// Checks to see if the requested variable is one of the ones which is stored
// in the OBJECTs section.	If so, see if it is present.  If not, return the default value.
//	If it is present, return the current value.
{
	db_dictionaryvar *dictvar;
	char *valstr;

	if ((dictvar=GetVarFromObjectList(found->name,dp))==NULL)
		valstr=found->defval;					// return the default value
	else {
		// EBATECH(CNBR) -[ 2002/10/10 Bugzilla78233 DXF=280 and 1
		//valstr=dictvar->ret_unkstr();					// return the actual value
		valstr=dictvar->ret_1();					// return the actual value
		// EBATECH(CNBR) ]-
	}
	// we don't have any reals right now
	ASSERT (found->type==RTSHORT || found->type==RTSTR);

	if (found->type==RTSHORT) {
		res->restype=RTSHORT;
		res->resval.rint=atoi(valstr);
	}
	else if (found->type==RTREAL) {
		res->restype=RTREAL;
		res->resval.rreal=atof(valstr);
	}
	else if (found->type==RTSTR) {
		res->restype=RTSTR;
		res->resval.rstring=NULL;
        db_astrncpy(&res->resval.rstring,valstr,SHRT_MAX);
	}

	return(RTNORM);
}


	// callback into ICAD code for validating serial number
bool	   (*db_serialNumber)( LPTSTR number, DWORD size) = NULL;
DB_API bool db_SetSerialNumber( pfnSerialNumber func )
	{
	ASSERT( CHECKFUNCPTR( func ) );

	db_serialNumber = func;

	return true;
	}

long julian(short day,short month,short year)  
{
  long c,ya;

  if (month>2) {
    month=(short)(month-3);
  }
  else {
         month = (short)(month+9);
         year = (short)(year-1);
       }
  c = year/100;
  ya = year-100 * c;
  return((long)(146097*c) / 4
                +(1461*ya) / 4
                +(153*month+2) / 5
                +day+1721119L);
} 

void encodeAcadDate(short day,short month,short year,short hour,short mins, short sec, short milliseconds,
                      long *longs)
{
  if (day==0 && month==0 && year==0 && hour==0 && mins==0 && sec==0) {
    longs[0]=longs[1]=0L;
    return;
  }
  longs[0]=julian(day,month,year);
  longs[1]=(long)hour*3600000L+(long)mins*60000L+(long)sec*1000L+(long)milliseconds;
}

short shortCodePageToAscii(short index, char *codepagestr, size_t cchcodepagestr)
{
  if (index<VALIDCODEPAGES) {
	  strncpy(codepagestr, dwgcodepagestrs[index], cchcodepagestr);
    return(1);
  }
  return(0);
}


/*F*/
int db_getvar(
	const char	   *vname,
	int 			qidx,
	resbuf		   *res,
	db_drawing	   *dp,
	db_charbuflink *configsv,
	db_charbuflink *sessionsv) {
/*
**	Searches for the system variable vname and sets *res to its value.
**	(res must point to an existing resbuf.)
**
**	NOTE: THIS FUNCTION WILL FREE AN EXISTING STRING VALUE IN res,
**	SO IF THE CALLER FREES IT BEFORE HAND, THE CALLER MUST SET
**	res->resval.rstring TO NULL.
**
**	THE CALLER MUST FREE res->resval.rstring IF THIS FUNCTION SETS
**	A STRING VALUE (res->restype==RTSTR && res->resval.rstring!=NULL).
**
**		vname : The name of the variable to locate
**		 qidx : Index into db_oldsysvar[] for quick access to certain
**				  heavily-used variables.  (Used when vname is NULL or ""
**				  only.)
**		  res : The result (value)
**	 configsv : The config system vars buffer to use
**	sessionsv : The session system vars buffer to use
**
**	(There are some hard-coded header locations in places that deal
**	with vars that require special handling (such as header vars that
**	are in a form not expected for its type).)
**
**	Returns:
**		 RTNORM : OK
**		RTERROR : Check msgn and the matching db_getsetvarmess[] string
**					for the reason.
*/
	char *whichbuf,strval[IC_ACADBUF],*fcp1,*fcp2,fc1;
	short shsz,arsz,lsz,strvalsz_1,ridx,msgn,fi1,fi2,fi3;
	long fl1;
	db_charbuflink *svbuf[3];
	db_sysvarlink *found = NULL;
	resbuf wcsrb,ucsrb,dcsrb;

	msgn=SETVAR_NO_ERROR;
	shsz=sizeof(short); arsz=sizeof(sds_real); lsz=sizeof(long);
	strvalsz_1=sizeof(strval)-1;
	*strval=strval[strvalsz_1]=0;

	wcsrb.rbnext=		 ucsrb.rbnext=		  dcsrb.rbnext =NULL;
	wcsrb.restype=		 ucsrb.restype= 	  dcsrb.restype=RTSHORT;
	wcsrb.resval.rint=0; ucsrb.resval.rint=1; dcsrb.resval.rint=2;


	/* Make an array of the sys var buffers for ease of use */
	/* with found->loc as the index: */
	svbuf[0]=(dp==NULL) ? NULL : &dp->header;
	svbuf[1]=configsv;
	svbuf[2]=sessionsv;

	if (res==NULL) { msgn=SETVAR_RESBUF_IS_NULL; goto out; }

	/* Free existing string value and set res to RTNIL as a safe default: */
	// Removed this free because we can't count on rstring being a valid allocated RTSTR.
	//if (res->restype==RTSTR &&)
	//	  { delete [] res->resval.rstring; res->resval.rstring=NULL; }

	res->resval.rstring=NULL;
	res->restype=RTNIL;

	if ((found=db_findsysvar(vname,qidx,dp))==NULL) { msgn=SETVAR_VARIABLE_NOT_FOUND; goto out; }

	/* For strings, to avoid duplicate allocation code, set strval */
	/* to the correct value and copy it later. */
	if (found->spl & IC_SYSVARFLAG_OBJDICTONARY) {  /* Variables are stored in object dictionaries. */
		if (GetSysvarFromObjects(found,dp,res)!=RTNORM) {
			msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out;
		}
		return(RTNORM);
	}
	else if (found->spl & IC_SYSVARFLAG_SPECIALSOURCE) {  /* Special source; not in a buffer. */

		if		  (!db_compareNames(found->name,"ACADPREFIX"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			*strval=0;
			if ((fcp1=getenv("ACAD"/*DNT*/))!=NULL) strncpy(strval,fcp1,strvalsz_1);
			else {
				struct resbuf rb;
				if(RTNORM==db_getvar(NULL,DB_QSRCHPATH,&rb,dp,configsv,sessionsv)) {
					strncpy(strval,rb.resval.rstring,strvalsz_1);
					delete [] rb.resval.rstring;
				}
			}
		} else if (!db_compareNames(found->name,"ACADVER"/*DNT*/)) {	 /* ° FIX */
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			strncpy(strval,"16.0i"/*DNT*/,strvalsz_1);
		} else if (!db_compareNames(found->name,"ICADVER"/*DNT*/)) {	 /* ° FIX */
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			strncpy(strval,found->defval,strvalsz_1);
		} else if (!db_compareNames(found->name,"CDATE"/*DNT*/) ||
				   !db_compareNames(found->name,"DATE"/*DNT*/)) {

			short td[6];
			char *months="JanFebMarAprMayJunJulAugSepOctNovDec"/*DNT*/;

			if (found->type!=RTREAL) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			res->resval.rreal=0.0;

			struct _timeb timebuffer;
			_ftime( &timebuffer );
			if ((fcp1=ctime( &(timebuffer.time)))!=NULL &&
				strlen(fcp1)>23) {
				td[0]=atoi(fcp1+20);						  /* Year */
				fc1=fcp1[7]; fcp1[7]=0;
				fcp2=strstr(months,fcp1+4);
				fcp1[7]=fc1;
				td[1]=((fcp2==NULL) ? 0 : (fcp2-months)/3)+1; /* Month */
				td[2]=atoi(fcp1+8); 						  /* Day */
				td[3]=atoi(fcp1+11);						  /* Hour */
				td[4]=atoi(fcp1+14);						  /* Minute */
				td[5]=atoi(fcp1+17);						  /* Second */
				if (!db_compareNames(found->name,"CDATE"/*DNT*/)) {
					/* Do string method to avoid rounding errors: */
					sprintf(strval,"%d%02d%02d.%02d%02d%02d%02d"/*DNT*/,
						td[0],td[1],td[2],td[3],td[4],td[5], (timebuffer.millitm/10) );
					res->resval.rreal=atof(strval);
				} else {  /* DATE */
					long	date[2];
					// EBATECH(FUTA) 2001-01-18 -[ get milliseconds
					//adEncodeAcadDate( td[2], td[1], td[0], td[3], td[4], td[5], date);
					encodeAcadDate( td[2], td[1], td[0], td[3], td[4], td[5],(short)timebuffer.millitm, date);
					// ]-
						// date[0] = julian date, date[1] = milliseconds of day
					res->resval.rreal=date[0] + date[1]/(24*60*60*1000.);
				}
			}
		} else if (!db_compareNames(found->name,"CMDNAMES"/*DNT*/)) {	  /* ° FIX */
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			*strval=0;
		} else if (!db_compareNames(found->name,"DWGNAME"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			*strval=0;
			if (dp!=NULL && dp->pn!=NULL) {
				// EBATECH(CNBR) for MBCS aware
				/* Find the 1st char of the filename: */
				//for (fi1=0; dp->pn[fi1]; fi1++);
				//while (fi1>-1 && dp->pn[fi1]!=IC_SLASH) fi1--;
				//fi1++;
				//
				///* Take it from there on: */
				//strncpy(strval,dp->pn+fi1,strvalsz_1);
				//strval[strvalsz_1]=0;
				TCHAR *ptr;
				if( ptr = _tcsrchr( dp->pn, IC_SLASH )){
					strncpy(strval,++ptr,strvalsz_1);
					strval[strvalsz_1]=0;
				}else{
					strncpy(strval,dp->pn,strvalsz_1);
					strcat(strval, IC_DWGEXT );
				}
				// EBATECH(CNBR)
			}
		} else if (!db_compareNames(found->name,"DWGPREFIX"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			// EBATECH(CNBR) for MBCS aware [
			//strncpy(strval,(dp==NULL) ? db_str_quotequote : dp->pn,strvalsz_1);
			//fi1=0; while (fi1<strvalsz_1 && strval[fi1]) fi1++;
			//while (fi1>-1 && strval[fi1]!=IC_SLASH) fi1--;
			//strval[fi1+1]=0;
			*strval = 0;
			if( dp!=NULL && dp->pn!=NULL ){
				strncpy(strval,dp->pn,strvalsz_1);
				TCHAR *ptr;
				if( ptr = _tcsrchr( strval, IC_SLASH )){
					*(++ptr) = 0;
				}else{
					*strval=0;
				}
			}
			if(*strval==0){
				struct resbuf rb;
				TCHAR	*ptr;
				if(RTNORM==db_getvar(NULL,DB_QSRCHPATH,&rb,dp,configsv,sessionsv)) {
					if(ptr=_tcschr(rb.resval.rstring,';')){
						*ptr=0;
						strncpy(strval,rb.resval.rstring,strvalsz_1);
						fi1 = strlen(strval);
						if(!( strval[fi1-2] == ':' && strval[fi1-1] == IC_SLASH)){
							strcat(strval, IC_SLASHSTR);
						}
					}else if(strlen(rb.resval.rstring)>0){
						strncpy(strval,rb.resval.rstring,strvalsz_1);
						fi1 = strlen(strval);
						if(!( strval[fi1-2] == ':' && strval[fi1-1] == IC_SLASH)){
							strcat(strval, IC_SLASHSTR);
						}
					}
					delete [] rb.resval.rstring;
				}
			}
			if(*strval==0){
				fi1 = (short)GetCurrentDirectory(IC_ACADBUF, strval );
				if(!( strval[fi1-2] == ':' && strval[fi1-1] == IC_SLASH)){
					strcat(strval, IC_SLASHSTR);
				}
			}
			// EBATECH(CNBR) ]
		} else if (!db_compareNames(found->name,"LOCALE"/*DNT*/)) { 	 /* ° FIX */
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			strncpy(strval,found->defval,strvalsz_1);
		} else if (!db_compareNames(found->name,"NODENAME"/*DNT*/)) {	   /* ° FIX */
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			strncpy(strval,found->defval,strvalsz_1);
		} else if (!db_compareNames(found->name,"PLATFORM"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			char *vers="Microsoft Windows %s Version %i.%i"/*DNT*/;
			OSVERSIONINFO vi;
			vi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
			GetVersionEx(&vi);
			switch(vi.dwPlatformId) {
				case VER_PLATFORM_WIN32s:		 sprintf(strval,vers,"3.1"/*DNT*/,vi.dwMajorVersion,vi.dwMinorVersion); break;
				case VER_PLATFORM_WIN32_WINDOWS: sprintf(strval,vers,"95"/*DNT*/,vi.dwMajorVersion,vi.dwMinorVersion);	break;
				case VER_PLATFORM_WIN32_NT: 	 sprintf(strval,vers,"NT"/*DNT*/,vi.dwMajorVersion,vi.dwMinorVersion);	break;
				default:						 sprintf(strval,vers,""/*DNT*/,vi.dwMajorVersion,vi.dwMinorVersion);	break;
			}
		} else if (!db_compareNames(found->name,"SAVENAME"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			*strval=0;
			if (dp!=NULL && dp->pn!=NULL) {
				strncpy(strval,dp->pn,strvalsz_1);
				strval[strvalsz_1]=0;
			}
		} else if (!db_compareNames(found->name,"SYSCODEPAGE"/*DNT*/)) {  /* ° FIX */
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			TCHAR	codePage[20];
			TCHAR	codePageName[24];
			int size = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, codePage, sizeof(codePage));
			strcat( strcpy( codePageName, "ANSI_"), codePage);
			strncpy(strval,codePageName,strvalsz_1);
		} else if (!db_compareNames(found->name,"_LINFO"/*DNT*/)) { 	 /* ° FIX */
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			strncpy(strval,found->defval,strvalsz_1);
		} else if (!db_compareNames(found->name,"_PKSER"/*DNT*/)) { 	  /* ° FIX */
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if ( db_serialNumber )
				(*db_serialNumber)( strval, strvalsz_1);
			else
				strval[0] = 0;
		} else if (!db_compareNames(found->name,"_SERVER"/*DNT*/)) {	  /* ° FIX */
			if (found->type!=RTSHORT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			res->resval.rint=-1;
		} else if (!db_compareNames(found->name,"CMDACTIVE"/*DNT*/))
			{
			// This is a very special variable that is dependent on global
			// state of the IntelliCAD engine.	As such it is set in SDS_getvar() at a much higher
			// level and should not be encountered down here
			//
			ASSERT( FALSE );
			msgn=SETVAR_BAD_TYPE_IN_DATABASE;
		// Added Cybage VSB 08/08/2001 [
		// Reason: Fix for Bug No: 77822
		}else if (!db_compareNames(found->name,"_VENDORNAME"/*DNT*/)) { 	 /* ° FIX */
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			strncpy(strval,found->defval,strvalsz_1);
		} // Added Cybage VSB 08/08/2001 ]
		else {
			msgn=SETVAR_SPECIAL_SOURCE_CASE_NOT_FOUND; goto out;
		}

	} 
	else {  /* Located in one of the buffers. */

			/* Make sure we have a buffer for this var and that we'll be */
			/* working in a valid region of it: */
		if (qidx==DB_QPLINETYPE)
			{
			qidx=DB_QPLINETYPE;
			}

		if (found->loc<0 || found->loc>2)
			{ msgn=SETVAR_BAD_BUFFER_IDX_IN_DATABASE; goto out; }
		if (svbuf[found->loc]==NULL || svbuf[found->loc]->buf==NULL)
			{ msgn=SETVAR_BUFFER_IS_NULL; goto out; }
		if (found->bc<0 || found->bc+found->nc-1>=svbuf[found->loc]->size)
			{ msgn=SETVAR_BAD_POSITION_IN_BUFFER; goto out; }

		/* Simplify the char buffer reference: */
		whichbuf=svbuf[found->loc]->buf;  /* (Just checked for NULL.) */

		if (!(found->spl & IC_SYSVARFLAG_UNUSUAL)) {
			/* No special problems; stored in buffer in format expected */
			/* for this ->type. */

			switch (found->type) {
				case RTSHORT:
					memcpy(&res->resval.rint,whichbuf+found->bc,shsz);
					break;
				case RTREAL: case RTANG: case RTORINT:
					memcpy(&res->resval.rreal,whichbuf+found->bc,arsz);
					break;
				case RTSTR:
					for (fi1=0; fi1<found->nc && whichbuf[found->bc+fi1]; fi1++);
					if (fi1>strvalsz_1) fi1=strvalsz_1;
					strncpy(strval,whichbuf+found->bc,fi1);
					break;
				case RTPOINT: case RT3DPOINT:
					memcpy(res->resval.rpoint,whichbuf+found->bc,arsz);
					memcpy(res->resval.rpoint+1,
						whichbuf+found->bc+arsz,arsz);
					if (found->type==RT3DPOINT)
						memcpy(res->resval.rpoint+2,
							whichbuf+found->bc+arsz+arsz,arsz);
					else res->resval.rpoint[2]=0.0;
					break;
				case RTLONG:
					memcpy(&res->resval.rlong,whichbuf+found->bc,lsz);
					break;
				default:
					msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out;
			}

		} 
		else {  /* Special: NOT stored in format expected. */

			/* CAREFUL!  SOME ORDER-DEPENDENT STUFF HERE.  */
			/* FIND strnicmp(). */

			if (!db_compareNames(found->name,"ELEVATION"/*DNT*/) ||
				!db_compareNames(found->name,"P_ELEVATION"/*DNT*/)) {
				sds_point ap1;

				if (found->type!=RTREAL) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

				/* Trans WCS origin to UCS and add the Z to the value */
				/* in the buffer: */
				ap1[0]=ap1[1]=ap1[2]=0.0;
				if (dp!=NULL) dp->trans(ap1,&wcsrb,&ucsrb,0,ap1,NULL);
				memcpy(&res->resval.rreal,whichbuf+found->bc,arsz);
				res->resval.rreal+=ap1[2];
			} else if (!db_compareNames(found->name,"CLAYER"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				if (dp==NULL || dp->current.clayer==NULL) *strval=0;
				else dp->current.clayer->get_2(strval,strvalsz_1);
			} else if (!db_compareNames(found->name,"CECOLOR"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				memcpy(&fi1,whichbuf+found->bc,shsz);
				if		(fi1==DB_BBCOLOR) strncpy(strval,db_str_byblock,strvalsz_1);
				else if (fi1==DB_BLCOLOR) strncpy(strval,db_str_bylayer,strvalsz_1);
				else			   sprintf(strval,"%d"/*DNT*/,fi1);
			} else if (!db_compareNames(found->name,"CELTYPE"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				if (dp==NULL || dp->current.celtype==NULL) *strval=0;
				else dp->current.celtype->get_2(strval,strvalsz_1);
						// force BYBLOCK/BYLAYER uppercase
				if ( strisame( strval, db_str_byblock) )
					_tcscpy( strval, db_str_byblock);
				else if ( strisame( strval, db_str_bylayer) )
					_tcscpy( strval, db_str_bylayer);
			} else if (!db_compareNames(found->name,"HANDSEED"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

				/* Convert from 8-byte form to string form in strval: */
				if (db_convhand(whichbuf+found->bc,strval,0,2))
					{ msgn=SETVAR_HANDLE_CONVERSION_FAILED; goto out; }
			} else if (!db_compareNames(found->name,"VIEWCTR"/*DNT*/)) {
				if (found->type!=RT3DPOINT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				memcpy(res->resval.rpoint,
					whichbuf+found->bc			,arsz);
				memcpy(res->resval.rpoint+1,
					whichbuf+found->bc+arsz 	,arsz);
				memcpy(res->resval.rpoint+2,
					whichbuf+found->bc+arsz+arsz,arsz);
				if (dp!=NULL) {
					dp->trans(res->resval.rpoint,&dcsrb,&ucsrb,0,res->resval.rpoint,NULL);
					dp->movept2elev(res->resval.rpoint,res->resval.rpoint);
				}
			} else if (!db_compareNames(found->name,"VIEWDIR"/*DNT*/)) {
				if (found->type!=RT3DPOINT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				memcpy(res->resval.rpoint,
					whichbuf+found->bc			,arsz);
				memcpy(res->resval.rpoint+1,
					whichbuf+found->bc+arsz 	,arsz);
				memcpy(res->resval.rpoint+2,
					whichbuf+found->bc+arsz+arsz,arsz);
				if (dp!=NULL) dp->trans(res->resval.rpoint,&wcsrb,&ucsrb,1,res->resval.rpoint,NULL);
			} else if (!db_compareNames(found->name,"CMLSTYLE"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

				*strval=0;

				/*
				**	° Replace the above line with something like
				**	  the code from CLAYER or TEXTSTYLE.
				*/
			} else if (!db_compareNames(found->name,"DIMSTYLE"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				if (dp==NULL || dp->current.dimstyle==NULL) strncpy(strval,"*UNNAMED"/*DNT*/,strvalsz_1);
				else dp->current.dimstyle->get_2(strval,strvalsz_1);
			} else if (!db_compareNames(found->name,"DIMTXSTY"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				if (dp==NULL || dp->current.dimtxsty==NULL) *strval=0;
				else dp->current.dimtxsty->get_2(strval,strvalsz_1);
			} else if (strnisame(found->name,"DIM"/*DNT*/,3)) {
				/* OTHER THAN DIMs ABOVE, the only DIMs with the spl flag */
				/* set are the 1-char flags: */
				if (found->type!=RTSHORT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				res->resval.rint = ((short)whichbuf[found->bc] & 0x00ff);
			} else if (!db_compareNames(found->name,"DWGCODEPAGE"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				memcpy(&ridx,whichbuf+found->bc,shsz);

								// get code page name or "undefined" if not valid
				TCHAR	codePage[20];
				if ( !shortCodePageToAscii( ridx, codePage, 20) )
					shortCodePageToAscii( 0, codePage, 20);

				_tcsupr( codePage); // display in upper case
				strncpy(strval,codePage,strvalsz_1);
			} else if (!db_compareNames(found->name,"MENUNAME"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				/* Get part 1: */
				for (fi1=0; fi1<found->nc && whichbuf[found->bc+fi1]; fi1++);
				if (fi1>strvalsz_1) fi1=strvalsz_1;
				strncpy(strval,whichbuf+found->bc,fi1);
				/* Add part 2: */
				fi3=508;  /* Where part 2 starts. */
				if (whichbuf[fi3]!=0) {
					for (fi1=0; fi1<45 && whichbuf[fi3+fi1]; fi1++);
					if (fi1>(fi2=strvalsz_1-strlen(strval))) fi1=fi2;
					if (fi1>0) strncat(strval,whichbuf+fi3,fi1);
				}
			} else if (!db_compareNames(found->name,"TARGET"/*DNT*/)	||
					   !db_compareNames(found->name,"LASTPOINT"/*DNT*/) ||
					   !db_compareNames(found->name,"LASTCMDPT"/*DNT*/) ||
					   !db_compareNames(found->name,"INSBASE"/*DNT*/)	||
					   !db_compareNames(found->name,"P_INSBASE"/*DNT*/)) {

				if (found->type!=RT3DPOINT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				memcpy(res->resval.rpoint,
					whichbuf+found->bc			,arsz);
				memcpy(res->resval.rpoint+1,
					whichbuf+found->bc+arsz 	,arsz);
				memcpy(res->resval.rpoint+2,
					whichbuf+found->bc+arsz+arsz,arsz);
				if (dp!=NULL) dp->trans(res->resval.rpoint,&wcsrb,&ucsrb,0,res->resval.rpoint,NULL);
			} else if (strnisame(found->name,"TD"/*DNT*/,2)) {
				sds_real toadd;

				/* Get the value from the buffer (no matter which TD it is): */
				if (found->type!=RTREAL) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				memcpy(&fl1,whichbuf+found->bc,lsz);
				res->resval.rreal=(sds_real)fl1;
				memcpy(&fl1,whichbuf+found->bc+lsz,lsz);
				res->resval.rreal+=((sds_real)fl1)/86400000.0;

				/* Determine the amount to add (if any): */
				toadd=0.0;
				if (!db_compareNames(found->name,"TDINDWG"/*DNT*/)) {
					toadd=(time(NULL)-dp->ret_sessionstarttime())/86400.0;
				} else if (!db_compareNames(found->name,"TDUSRTIMER"/*DNT*/)) {
					/* See if the user-timer is on: */
					memcpy(&fi1,whichbuf+db_oldsysvar[DB_QUSRTIMER].bc,shsz);
					if (fi1) toadd=(time(NULL)-dp->ret_usrstarttime())/86400.0;
				}

				res->resval.rreal+=toadd;
			} else if (!db_compareNames(found->name,"TEXTSTYLE"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				if (dp==NULL || dp->current.textstyle==NULL) *strval=0;
				else dp->current.textstyle->get_2(strval,strvalsz_1);
			} else if (!db_compareNames(found->name,"UCSFOLLOW"/*DNT*/)) {
				if (found->type!=RTSHORT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				memcpy(&ridx,whichbuf+found->bc,shsz);
				res->resval.rint=((ridx&8)!=0);
			} else if (!db_compareNames(found->name,"UCSNAME"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				if (dp==NULL || dp->current.ucsname==NULL) *strval=0;
				else dp->current.ucsname->get_2(strval,strvalsz_1);
			} else if (!db_compareNames(found->name,"P_UCSNAME"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				if (dp==NULL || dp->current.p_ucsname==NULL) *strval=0;
				else dp->current.p_ucsname->get_2(strval,strvalsz_1);
			}
			else if (!db_compareNames(found->name,"CPLOTSTYLE"/*DNT*/) ||
				!db_compareNames(found->name,"DEFLPLSTYLE"/*DNT*/) ||
				!db_compareNames(found->name,"DEFPLSTYLE"/*DNT*/)) {
				if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
				int pstylemode;
				char *buf = ( dp == NULL ) ? NULL : dp->ret_headerbuf();
				if(db_qgetvar(DB_QPSTYLEMODE, buf, &pstylemode, DB_INT, 0)!=0)
					{ msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out;}
				if(pstylemode==DB_BYCOL_PSTYLEMODE)
					strcpy(strval,db_str_bycolor);
				else
					strncpy(strval,whichbuf+found->bc,strvalsz_1);
			}
			else {
				msgn=SETVAR_SPECIAL_FORMAT_CASE_NOT_FOUND; goto out;
			}

		}  /* End else NOT stored in format expected. */
	}  /* End else located in one of the buffers. */

	res->restype=found->type;  /* Set the type. */

	/* If it's a string, take the value set up in strval: */
	if (res->restype==RTSTR) {
		res->resval.rstring=NULL;
		db_mstrncpy(&res->resval.rstring,strval,SHRT_MAX);
	}

out:

	#if defined(DEBUG)
		if (msgn>SETVAR_NO_ERROR && msgn<sizeof(db_getsetvarmess)/sizeof(db_getsetvarmess[0])) {
			char *namep,fs1[161];

			if (vname!=NULL && *vname)		   namep=(char *)vname;
			else if (qidx>-1 && qidx<db_nvars) namep=db_oldsysvar[qidx].name;
			else							   namep=db_str_invalid;

			sprintf(fs1,"Getvar \"%s\"(%d): %s.\n"/*DNT*/,namep,qidx,db_getsetvarmess[msgn]);
			if (db_errormessfnp!=NULL) (*db_errormessfnp)(fs1,0);

		}
	#endif

	return (msgn>SETVAR_NO_ERROR) ? RTERROR : RTNORM;
}



// This global variable and function return the last setvar that we set.

db_sysvarlink *db_LastFoundSetvar;

/*F*/
DB_API db_sysvarlink *db_GetLastSetvar(void) { return(db_LastFoundSetvar); }


int SetSysvarInObjects(	db_sysvarlink *found, db_drawing *dp, const resbuf *res)
// Checks to see if the requested variable is one of the ones which is stored
// in the OBJECTs section.	If so, store it there; if not, return RTERROR
{
	db_dictionaryvar *dictvar;
	db_handitem *stobj,*endobj;
	db_dictionaryrec *maindicvbldicrec,*thevbldicrec;
	db_dictionary *vbldic,*maindic;
	sds_name vbldictname,dictionaryvarname;
	int ret;
	char varbuf[IC_ACADBUF];
	char *vbptr=varbuf;
	struct resbuf *vbldicrb;

	if (found->type==RTSTR && (int)strlen(res->resval.rstring) > found->nc) return RTERROR;

	dp->get_objllends(&stobj,&endobj);

	// when main dictionary is not present, rise an error. / Ebatech(cnbr) 2002/9/16
	if(!stobj){ ASSERT(FALSE); return RTERROR; }
	maindic=(db_dictionary *)stobj;

	// we assume that the first object is the main dictionary item
	// find the dicrec that points to the variable dictionary, if present
	maindicvbldicrec=((db_dictionary *)stobj)->findrec("ACDBVARIABLEDICTIONARY"/*DNT*/,0,dp);
	if (maindicvbldicrec==NULL) { // not present, so

		// create the variable dictionary
		vbldicrb=sds_buildlist(RTDXF0,"DICTIONARY"/*DNT*/,0);
		ret=dp->entmake(vbldicrb,(class db_handitem **)&vbldictname[0]);
		sds_relrb(vbldicrb);
		if (ret==RTERROR) return RTERROR;

		vbldictname[1]=(long)dp;
		vbldic=(db_dictionary *)(vbldictname[0]);

		// now put a pointer to it into the main dictionary
		maindicvbldicrec=new db_dictionaryrec("ACDBVARIABLEDICTIONARY",
					DB_SOFT_OWNER,vbldic->RetHandle(),vbldic);
		if (maindicvbldicrec==NULL) {
			dp->delhanditem(NULL,vbldic,0);
			return(RTERROR);
		}

		if (!vbldic->addReactor(maindic->RetHandle(), maindic, ReactorItem::SOFT_POINTER))
		{
			dp->delhanditem(NULL,vbldic,0);
			delete maindicvbldicrec;
			return(RTERROR);
		}
		maindic->addrec(maindicvbldicrec);
	}
	else vbldic=(db_dictionary *)maindicvbldicrec->hiref->ret_hip(dp);

	// at this point we have a valid variable dictionary and a valid pointer
	// to it in the main dictionary, so we DO NOT have to clean these up if
	// we have further failures

	// now search the variable dictionary for the desired system variable
	if ((thevbldicrec=vbldic->findrec(found->name,0,dp))==NULL) {

		// not found, so add a dictionaryvar object and a record to point to it
		struct resbuf *newobj=sds_buildlist(RTDXF0,"DICTIONARYVAR"/*DNT*/,0);
		ret=dp->entmake(newobj,(class db_handitem **)&dictionaryvarname[0]);
		sds_relrb(newobj);
		if (ret==RTERROR) return RTERROR;

		dictionaryvarname[1]=(long)dp;
		dictvar=(db_dictionaryvar *)(dictionaryvarname[0]);
		thevbldicrec=new db_dictionaryrec(found->name,DB_SOFT_OWNER,dictvar->RetHandle(),dictvar);
		if (thevbldicrec==NULL) {
			dp->delhanditem(NULL,dictvar,0);
			return(RTERROR);
		}
		if (!dictvar->addReactor(vbldic->RetHandle(), vbldic, ReactorItem::SOFT_POINTER))
		{
			dp->delhanditem(NULL,dictvar,0);
			delete thevbldicrec;
			return(RTERROR);
		}
		vbldic->addrec(thevbldicrec);
	}
	else dictvar=(db_dictionaryvar *)thevbldicrec->hiref->ret_hip(dp);

	if (found->type==RTSHORT) {
		sprintf(varbuf,"%-d",res->resval.rint);
	}
	else if (found->type==RTREAL) {
		sprintf(varbuf,"%f",res->resval.rreal);
	}
	else if (found->type==RTSTR) {
		vbptr=res->resval.rstring;
	}

    // EBATECH(CNBR) -[ 2002/10/10 Bugzilla78233 DXF=280 and 1
	//dictvar->set_unkstr(vbptr);
	dictvar->set_1(vbptr);
	// EBATECH(CNBR) ]-
	return(RTNORM);
}


// EBATECH(watanabe)-[dimfit, dimunit
static
BOOL
SetOtherVars(
  const char*		vname,
  const int 		qidx,
  db_drawing*		dp,
  const char*		whichbuf,
  db_sysvarlink*	found,
  const int 		value
)
{
  int shsz = sizeof(short);
  //EBATECH(CNBR) Remove CString
  //CString str;
  char* str;

  if (qidx == -1)
	{
	  //str = vname;
	  str = (char*)vname;
	}
  else
	{
	  str = found->name;
	}

  //if (str.CompareNoCase("DIMFIT") == 0)
  if (stricmp(str,"DIMFIT"/*DNT*/) == 0)
	{
	  int fit = value;

	  // set DIMATFIT
	  int atfit = fit;
	  if (fit > 3)
		{
		  atfit = 3;
		}
	  db_sysvarlink* pDimatfit = db_findsysvar("DIMATFIT", -1, dp);
	  memcpy((void*)(whichbuf + pDimatfit->bc), &atfit, shsz);

	  // set DIMTMOVE
	  int tmove = 0;
	  if (fit > 3)
		{
		  tmove = fit - 3;
		}
	  db_sysvarlink* pDimtmove = db_findsysvar("DIMTMOVE", -1, dp);
	  memcpy((void*)(whichbuf + pDimtmove->bc), &tmove, shsz);
	}
  //else if (str.CompareNoCase("DIMATFIT") == 0)
  else if (stricmp(str,"DIMATFIT"/*DNT*/) == 0)
	{
	  int atfit = value;

	  // get DIMTMOVE
	  db_sysvarlink* pDimtmove = db_findsysvar("DIMTMOVE", -1, dp);
	  int tmove;
	  memcpy(&tmove, whichbuf + pDimtmove->bc, shsz);
	  // EBATECH(watanabe)-[invalid value
	  if ((tmove < 0) || (tmove > 2))
		{
		  tmove = 0;
		}
	  // ]-EBATECH(watanabe)

	  // set DIMFIT
	  int fit = atfit;
	  if (tmove == 1)
		{
		  fit = 4;
		}
	  else if (tmove == 2)
		{
		  fit = 5;
		}
	  db_sysvarlink* pDimfit = db_findsysvar("DIMFIT", -1, dp);
	  memcpy((void*)(whichbuf + pDimfit->bc), &fit, shsz);
	}
  //else if (str.CompareNoCase("DIMTMOVE") == 0)
  else if (stricmp(str,"DIMTMOVE"/*DNT*/) == 0)
	{
	  int tmove = value;

	  // get DIMATFIT
	  db_sysvarlink* pDimatfit = db_findsysvar("DIMATFIT", -1, dp);
	  int atfit;
	  memcpy(&atfit, whichbuf + pDimatfit->bc, shsz);
	  // EBATECH(watanabe)-[invalid value
	  if ((atfit < 0) || (atfit > 5))
		{
		  // SystemMetrix(Hiro)-[FIX: Change initial value for DIMATFIT
		  //atfit = 0;
		  atfit = 3;
		  // ]-SystemMetrix(Hiro) FIX: Change initial value for DIMATFIT
		}
	  // ]-EBATECH(watanabe)

	  // set DIMFIT
	  int fit = atfit;
	  if (tmove == 1)
		{
		  fit = 4;
		}
	  else if (tmove == 2)
		{
		  fit = 5;
		}
	  db_sysvarlink* pDimfit = db_findsysvar("DIMFIT", -1, dp);
	  memcpy((void*)(whichbuf + pDimfit->bc), &fit, shsz);
	}
  //else if (str.CompareNoCase("DIMUNIT") == 0)
  else if (stricmp(str,"DIMUNIT"/*DNT*/) == 0)
	{
	  int unit = value;

	  // set DIMLUNIT
	  int lunit = unit;
	  if (unit >= 6)
		{
		  lunit -= 2;
		}
	  db_sysvarlink* pDimlunit = db_findsysvar("DIMLUNIT", -1, dp);
	  memcpy((void*)(whichbuf + pDimlunit->bc), &lunit, shsz);

	  switch (unit)
		{
		  case 4:
		  case 5:
		  case 6:
		  case 7:
			  // set DIMFRAC
			  int frac = 0;
			  if ((unit == 6) ||
				  (unit == 7))
				{
				  frac = 2;
				}
			  db_sysvarlink* pDimfrac = db_findsysvar("DIMFRAC", -1, dp);
			  memcpy((void*)(whichbuf + pDimfrac->bc), &frac, shsz);
			  break;
		}
	}
  //else if (str.CompareNoCase("DIMLUNIT") == 0)
  else if (stricmp(str,"DIMLUNIT"/*DNT*/) == 0)
	{
	  int lunit = value;

	  // get DIMFRAC
	  db_sysvarlink* pDimfrac = db_findsysvar("DIMFRAC", -1, dp);
	  int frac;
	  memcpy(&frac, whichbuf + pDimfrac->bc, shsz);
	  // EBATECH(watanabe)-[invalid value
	  if ((frac < 0) || (frac > 2))
		{
		  frac = 0;
		}
	  // ]-EBATECH(watanabe)

	  // set DIMUNIT
	  int unit = lunit;
	  switch (unit)
		{
		  case 4:
		  case 5:
			  if (frac == 2)
				{
				  unit += 2;
				}
			  break;
		  case 6:
			  unit += 2;
			  break;
		}
	  db_sysvarlink* pDimunit = db_findsysvar("DIMUNIT", -1, dp);
	  memcpy((void*)(whichbuf + pDimunit->bc), &unit, shsz);
	}
  //else if (str.CompareNoCase("DIMFRAC") == 0)
  else if (stricmp(str,"DIMFRAC"/*DNT*/) == 0)
	{
	  int frac = value;

	  // get DIMLUNIT
	  db_sysvarlink* pDimlunit = db_findsysvar("DIMLUNIT", -1, dp);
	  int lunit;
	  memcpy(&lunit, whichbuf + pDimlunit->bc, shsz);
	  // EBATECH(watanabe)-[invalid value
	  if ((lunit < 0) || (lunit > 6))
		{
		  lunit = 0;
		}
	  // ]-EBATECH(watanabe)

	  // set DIMUNIT
	  int unit = lunit;
	  switch (unit)
		{
		  case 4:
		  case 5:
			  if (frac == 2)
				{
				  unit += 2;
				}
			  break;
		  case 6:
			  unit += 2;
			  break;
		}
	  db_sysvarlink* pDimunit = db_findsysvar("DIMUNIT", -1, dp);
	  memcpy((void*)(whichbuf + pDimunit->bc), &unit, shsz);
	}
	// EBATECH(CNBR) ]- remove CString
  return TRUE;
}
// ]-EBATECH(watanabe)

short asciiCodePageToShort(const char *codepagestr,short *index)
{
  short i;

  for (i=0; i<VALIDCODEPAGES; i++) {
    if (0 == _stricmp(dwgcodepagestrs[i],codepagestr)) {
      *index=i;
      return(1);
    }
  }
  return(0);
}

/*F*/
int db_setvar(
	const char	   *vname,
	int 			qidx,
	const resbuf   *argsour,
	db_drawing	   *dp,
	db_charbuflink *configsv,
	db_charbuflink *sessionsv,
	int 			asis) {
/*
**	Searches for the system variable vname and sets its value to
**	the value *sour specifies.	(*sour must have the proper RTtype
**	for vname.)  The value is stored in the appropriate char buffer.
**
**		vname : The name of the variable to locate
**		 qidx : Index into db_sysvar[] for quick access to certain
**				  heavily-used variables.  (Used when vname is NULL or ""
**				  only.)
**	  argsour : The value to set
**	 configsv : The config system vars buffer to use
**	sessionsv : The session system vars buffer to use
**		 asis : Flags this function to store the value without
**				  transformation.  (Some of the view-related vars
**				  (and ELEVATION) normally require modification
**				  before they are stored because the header
**				  keeps them in a form that's different that the
**				  form in which they are exchanged with a user.
**				  HOWEVER, when we read an R13 DWG header and *ACTIVE
**				  vport record, we want to setvar values which are
**				  already in the correct form.)
**
**	(There are some hard-coded header locations in places that deal
**	with vars that require special handling (such as header vars that
**	are in a form not expected for its type).)
**
**	Byte-order BS is performed in storing values in all of the
**	buffers (to keep all buffers like the header buffer).
**
**	Returns:
**		 RTNORM : OK
**		RTERROR : Check msgn and the matching db_getsetvarmess[] string
**					for the reason.
*/
	char *whichbuf,fs1[IC_ACADBUF];
	int shsz,lsz,arsz,fs1sz_1,msgn,fi1,fi2;
	short fshort1;
	unsigned short fushort1;
	long fl1;
	sds_real ar1,ar2;
	sds_point ap1;
	db_charbuflink *svbuf[3];
	db_sysvarlink *found;
	resbuf wcsrb,ucsrb,dcsrb;
	db_handitem *thip1;


	char *colorstr[]=
		{ "RED"/*DNT*/,"YELLOW"/*DNT*/,"GREEN"/*DNT*/,"CYAN"/*DNT*/,"BLUE"/*DNT*/,"MAGENTA"/*DNT*/,"WHITE"/*DNT*/ };

	msgn=SETVAR_NO_ERROR;
	shsz=sizeof(short); arsz=sizeof(sds_real); lsz=sizeof(long);
	fs1sz_1=sizeof(fs1)-1;

	wcsrb.rbnext=		 ucsrb.rbnext=		  dcsrb.rbnext =NULL;
	wcsrb.restype=		 ucsrb.restype= 	  dcsrb.restype=RTSHORT;
	wcsrb.resval.rint=0; ucsrb.resval.rint=1; dcsrb.resval.rint=2;

	/* Make an array of the sys var buffers for ease of use */
	/* with found->loc as the index: */
	svbuf[0]=(dp==NULL) ? NULL : &dp->header;
	svbuf[1]=configsv;
	svbuf[2]=sessionsv;

	if (argsour==NULL) { msgn=SETVAR_RESBUF_IS_NULL; goto out; }

	resbuf source = *argsour;

	/* Search db_oldsysvar[]: */
	db_LastFoundSetvar=NULL;
	if ((found=db_findsysvar(vname,qidx,dp))==NULL) { msgn=SETVAR_VARIABLE_NOT_FOUND; goto out; }
	db_LastFoundSetvar=found;

	/* Copy the argsour into sour so we can accept RTREALs as */
	/* RTSHORTs, RTLONGs etc.  This trys to be more forgiving */
	/* to users that call setvar with the wrong type set.	  */
	if (found->spl & IC_SYSVARFLAG_OBJDICTONARY) {

		if (SetSysvarInObjects(found,dp,argsour)==RTNORM) {
			db_LastFoundSetvar = db_findsysvar(vname, qidx, dp);	/*D.G.*/// Set it for undo/redo.
			return(RTNORM);
		}

		// If the object variable is not handled correctly then bail
		goto out;
	}

	if(found->type==RTSHORT) {
		if(source.restype==RTREAL) {
			source.restype=RTSHORT;
			source.resval.rint=(int)source.resval.rreal;
		}
		if(source.restype==RTLONG) {
			source.restype=RTSHORT;
			source.resval.rint=(int)source.resval.rlong;
		}
		//Modified Cybage AJK  27/8/2001 [
		//Reason : Fix for Bug No. 77833 from Bugzilla
		//if(source.restype==RTSTR) {
		//	source.restype=RTSHORT;
		//	source.resval.rint=atoi(source.resval.rstring);
		//}
		//Modified Cybage AJK  27/8/2001 ]
	}
	if(found->type==RTREAL) {
		if(source.restype==RTSHORT) {
			source.restype=RTREAL;
			source.resval.rreal=(double)source.resval.rint;
		}
		if(source.restype==RTLONG) {
			source.restype=RTREAL;
			source.resval.rreal=(double)source.resval.rlong;
		}
		//Modified Cybage AJK  27/8/2001 [
		//Reason : Fix for Bug No. 77833 from Bugzilla
		//if(source.restype==RTSTR) {
		//	source.restype=RTREAL;
		//	source.resval.rreal=atof(source.resval.rstring);
		//}
		//Modified Cybage AJK  27/8/2001 ]
		// EBATECH(FUTA)-[ 2001-01-08 Fix "(setvar "SNAPANG" (getangle))" don't work.
		if(source.restype==RTANG) {
			source.restype=RTREAL;
		}
		// ]-EBATECH(FUTA)
	}

	/* Check source type with what's needed (but let 2D/3D point */
	/* discrepancies pass): */
	if (found->type!=source.restype &&
		((found->type	!=RTPOINT && found->type   !=RT3DPOINT) ||
		 ( source.restype!=RTPOINT &&  source.restype!=RT3DPOINT)))
			{ msgn=SETVAR_TYPE_MISMATCH; goto out; }

	/* See if special source (not in a buf -- usually truly read-only): */
	if (found->spl & IC_SYSVARFLAG_SPECIALSOURCE) {
		if (!db_compareNames(found->name,"DWGNAME"/*DNT*/)) {  /* A special exception */
			if (source.restype!=RTSTR) { msgn=SETVAR_TYPE_MISMATCH; goto out; }
			if (dp==NULL)			  { msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
			if (source.resval.rstring!=NULL)
				db_astrncpy(&dp->pn,source.resval.rstring,DB_PATHNAMESZ-1);
		} else msgn=SETVAR_TRUE_READ_ONLY;
		goto out;
	}

	/* Located in one of the buffers, if got to here. */

	/* Make sure we have a buffer for this var and that we'll be */
	/* working in a valid region of it: */
	if (found->loc<0 || found->loc>2)
		{ msgn=SETVAR_BAD_BUFFER_IDX_IN_DATABASE; goto out; }
	if (svbuf[found->loc]==NULL || svbuf[found->loc]->buf==NULL)
		{ msgn=SETVAR_BUFFER_IS_NULL; goto out; }
	if (found->bc<0 || found->bc+found->nc-1>=svbuf[found->loc]->size)
		{ msgn=SETVAR_BAD_POSITION_IN_BUFFER; goto out; }

	/* Check the range: */
	if (db_chkrange(&source,found->range)) { msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }

	/* Simplify the char buffer reference: */
	whichbuf=svbuf[found->loc]->buf;  /* (Just checked for NULL.) */

	if (found->spl & (IC_SYSVARFLAG_UNUSUAL | IC_SYSVARFLAG_SPECIALHANDLING | IC_SYSVARFLAG_SPECIALRANGE)) {
		/* Special form in buffer, or not independent, or special range */

		/* CAREFUL!  SOME ORDER-DEPENDENT STUFF HERE.  */
		/* FIND strnicmp(). */

		if(!db_compareNames(found->name,"CELWEIGHT"/*DNT*/)||
			!db_compareNames(found->name,"DIMLWD"/*DNT*/)||
			!db_compareNames(found->name,"DIMLWE"/*DNT*/)) {
			if (found->type!=RTSHORT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if( db_is_validweight( source.resval.rint, (DB_BLWMASK|DB_BBWMASK|DB_DFWMASK)) == false ) {
				msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
			memcpy(whichbuf+found->bc,&source.resval.rint,shsz);
		} else if  (!db_compareNames(found->name,"LWDEFAULT"/*DNT*/)) {
			if (found->type!=RTSHORT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if( db_is_validweight( source.resval.rint, 0) == false ) {
				msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
			memcpy(whichbuf+found->bc,&source.resval.rint,shsz);
		}
		// 2002/9/30 CTAB has special range
		else if (!db_compareNames(found->name,"CTAB"/*DNT*/)) {
			db_objhandle hand;
			db_handitem *dictp;
			if (found->type!=RTSTR)
			{ msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			strncpy(fs1,source.resval.rstring,fs1sz_1); ic_trim(fs1,"be"/*DNT*/);
			if( strisame(fs1, "Model"/*DNT*/))
			{ goto ctab_okay; }
			hand = dp->ret_stockhand( DB_SHI_LAYOUTDIC );
			if( hand == NULL )
			{ msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
			if((dictp = dp->handent( hand )) == NULL )
			{ msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
			if( dp->dictsearch( dictp, fs1, 0 ) == NULL )
			{ msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
		ctab_okay:
			strncpy(whichbuf+found->bc,fs1,found->nc-1);
		}
		// 2002/9/30 CPLOTSTYLE has special range.
		else if (!db_compareNames(found->name,"CPLOTSTYLE"/*DNT*/)){
			db_objhandle hand;
			db_handitem* dictp;
			// check request data type and trim string.
			if( found->type!=RTSTR ) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			strncpy(fs1,source.resval.rstring,fs1sz_1); ic_trim(fs1,"be"/*DNT*/);
			// database is bycolor?
			int pstylemode;
			char *buf = ( dp == NULL ) ? NULL : dp->ret_headerbuf();
			if(db_qgetvar(DB_QPSTYLEMODE, buf, &pstylemode, DB_INT, 0)!=0)
			{ msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out;}
			if(pstylemode==DB_BYCOL_PSTYLEMODE)
			{ msgn=SETVAR_TRUE_READ_ONLY; goto out;}
			// CPLOTSTYLE accepts BYLAYER, BYBLOCK and Good value in dictionary.
			if( !db_compareNames(fs1, db_str_bycolor))
			{ msgn=SETVAR_TRUE_READ_ONLY; goto out;}
			if( !db_compareNames(fs1, db_str_bylayer)) goto cplotstyle_okay;
			if( !db_compareNames(fs1, db_str_byblock)) goto cplotstyle_okay;
			if (dp==NULL) { msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
			hand = dp->ret_stockhand( DB_SHI_DICWDFLT );
			if( hand == NULL )
			{ msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
			dictp = dp->handent( hand );
			if( dictp == NULL || dp->dictsearch( dictp, fs1, 0 ) == NULL )
			{ msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
cplotstyle_okay:
			strncpy(whichbuf+found->bc,fs1,found->nc-1);
		// 2002/9/12 DEFLPLSTYLE and DEFPLSTYLE cannot change value.
		} else if( !db_compareNames(found->name,"DEFLPLSTYLE"/*DNT*/) ||
			!db_compareNames(found->name,"DEFPLSTYLE"/*DNT*/)){
			msgn=SETVAR_TRUE_READ_ONLY;
			goto out;
		}
		 else if (!db_compareNames(found->name,"ELEVATION"/*DNT*/) ||
				   !db_compareNames(found->name,"P_ELEVATION"/*DNT*/)) {

			if (found->type!=RTREAL) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			ar1=source.resval.rreal;
			if (!asis) {
				/* Store the elevation minus the Z-coord of the WCS origin */
				/* as seen by the UCS: */
				ap1[0]=ap1[1]=ap1[2]=0.0;
				if (dp==NULL) { msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
				dp->trans(ap1,&wcsrb,&ucsrb,0,ap1,NULL);
				ar1-=ap1[2];
			}
			memcpy(whichbuf+found->bc,&ar1,arsz);
		} else if (!db_compareNames(found->name,"CLAYER"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if (dp==NULL)			{ msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
			if ((thip1=dp->findtabrec(DB_LAYERTAB,source.resval.rstring,1))==NULL)
				{ msgn=SETVAR_TABLE_RECORD_NOT_FOUND; goto out; }
			dp->current.clayer=thip1;
		} else if (!db_compareNames(found->name,"CECOLOR"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			strncpy(fs1,source.resval.rstring,fs1sz_1); ic_trim(fs1,"be"/*DNT*/);
			if		(!db_compareNames(fs1,db_str_bylayer)) fshort1=DB_BLCOLOR;
			else if (!db_compareNames(fs1,db_str_byblock)) fshort1=DB_BBCOLOR;
			else if (isdigit((unsigned char) *fs1)) {
				if ((fshort1=atoi(fs1))<0 || fshort1>256) { msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
			} else {
				for (fshort1=0; fshort1<7 && db_compareNames(fs1,colorstr[fshort1]); fshort1++);
				if (++fshort1>7) { msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
			}
			memcpy(whichbuf+found->bc,&fshort1,shsz);
		} else if (!db_compareNames(found->name,"CELTYPE"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if (dp==NULL)			{ msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
			if ((thip1=dp->findtabrec(DB_LTYPETAB,source.resval.rstring,1))==NULL)
				{ msgn=SETVAR_TABLE_RECORD_NOT_FOUND; goto out; }
			dp->current.celtype=thip1;
		} else if (!db_compareNames(found->name,"HANDSEED"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			/* Convert string to 8-byte form in buffer: */
			if (db_convhand(source.resval.rstring,whichbuf+found->bc,2,0))
				{ msgn=SETVAR_HANDLE_CONVERSION_FAILED; goto out; }
		} else if (!db_compareNames(found->name,"VIEWCTR"/*DNT*/)) {
			if (found->type!=RT3DPOINT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			ic_ptcpy(ap1,source.resval.rpoint);
			if (!asis) {
				if (dp==NULL) { msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
				dp->trans(ap1,&ucsrb,&dcsrb,0,ap1,NULL);
				ap1[2]=0.0;  /* Make it lie in the DCS XY-plane. */
			}
			memcpy(whichbuf+found->bc		   ,ap1  ,arsz);
			memcpy(whichbuf+found->bc+arsz	   ,ap1+1,arsz);
			memcpy(whichbuf+found->bc+arsz+arsz,ap1+2,arsz);
		} else if (!db_compareNames(found->name,"VIEWDIR"/*DNT*/)	||
				   !db_compareNames(found->name,"UCSXDIR"/*DNT*/)	||
				   !db_compareNames(found->name,"UCSYDIR"/*DNT*/)	||
				   !db_compareNames(found->name,"P_UCSXDIR"/*DNT*/) ||
				   !db_compareNames(found->name,"P_UCSYDIR"/*DNT*/)) {
			if (found->type!=RT3DPOINT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			/* These are vectors; disallow (0,0,0). */
			if (icadRealEqual(source.resval.rpoint[0],0.0) &&
				icadRealEqual(source.resval.rpoint[1],0.0) &&
				icadRealEqual(source.resval.rpoint[2],0.0)) { msgn=SETVAR_ZERO_VECTOR_NOT_ALLOWED; goto out; }

			ic_ptcpy(ap1,source.resval.rpoint);
			if (!db_compareNames(found->name,"VIEWDIR"/*DNT*/) && !asis) {
				if (dp==NULL) { msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
				dp->trans(ap1,&ucsrb,&wcsrb,1,ap1,NULL);
			}
			memcpy(whichbuf+found->bc		   ,ap1  ,arsz);
			memcpy(whichbuf+found->bc+arsz	   ,ap1+1,arsz);
			memcpy(whichbuf+found->bc+arsz+arsz,ap1+2,arsz);
		} else if ((fshort1=db_compareNames(found->name,"GRIDUNIT"/*DNT*/))==0 ||
					   !db_compareNames(found->name,"SNAPUNIT"/*DNT*/)) {

			if (found->type!=RTPOINT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if (( fshort1 && (source.resval.rpoint[0]<=0.0 ||
						  source.resval.rpoint[1]<=0.0)) ||
				(!fshort1 && (source.resval.rpoint[0]<0.0 ||
						  source.resval.rpoint[1]<0.0))) { msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
			memcpy(whichbuf+found->bc	  ,source.resval.rpoint  ,arsz);
			memcpy(whichbuf+found->bc+arsz,source.resval.rpoint+1,arsz);
		} else if (!db_compareNames(found->name,"CMLSTYLE"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if (dp==NULL)			{ msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }

			dp->current.cmlstyle=NULL;

			/*
			**	° Replace the above line with something like
			**	  the code for TEXTSTYLE.
			*/
		} else if (!db_compareNames(found->name,"DCTCUST"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			/* ° FIX SOMEDAY TO CHECK FOR VALID SPELLING DICTIONARY */
			/*	   PATHNAME. */

			strncpy(whichbuf+found->bc,source.resval.rstring,found->nc-1);

		} else if (!db_compareNames(found->name,"DCTMAIN"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			/* ° FIX SOMEDAY TO USE THE KEYWORDS AND CHECK FOR VALID */
			/*	 SPELLING DICTIONARY FILENAME. */

			strncpy(whichbuf+found->bc,source.resval.rstring,found->nc-1);

		} else if (!db_compareNames(found->name,"DIMSTYLE"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if (dp==NULL)			{ msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
			if (!db_compareNames(source.resval.rstring,"*UNNAMED"/*DNT*/)) dp->current.dimstyle=NULL;
			else {
				if ((thip1=dp->findtabrec(DB_DIMSTYLETAB,source.resval.rstring,1))==NULL)
					{ msgn=SETVAR_TABLE_RECORD_NOT_FOUND; goto out; }
				dp->current.dimstyle=thip1;
			}
		} else if (!db_compareNames(found->name,"DIMTXSTY"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if (dp==NULL)			{ msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
			if ((thip1=dp->findtabrec(DB_STYLETAB,source.resval.rstring,1))==NULL)
				{ msgn=SETVAR_TABLE_RECORD_NOT_FOUND; goto out; }
			dp->current.dimtxsty=thip1;
		} else if (strnisame(found->name,"DIM",3)) {
			/* OTHER THAN DIMs ABOVE, the only DIMs with the spl flag */
			/* set are the 1-char flags: */
			if (found->type!=RTSHORT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			whichbuf[found->bc]=(char)source.resval.rint;
		} else if (!db_compareNames(found->name,"DWGCODEPAGE"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			strncpy(fs1,source.resval.rstring,fs1sz_1); ic_trim(fs1,"be"/*DNT*/);
			if ( !asciiCodePageToShort( fs1, &fshort1) )
				fshort1 = 0;
			memcpy(whichbuf+found->bc,&fshort1,shsz);
		} else if (!db_compareNames(found->name,"FONTALT"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			/* ° FIX SOMEDAY TO CHECK FOR VALID FONT NAME. */

			strncpy(whichbuf+found->bc,source.resval.rstring,found->nc-1);

		} else if (!db_compareNames(found->name,"FONTMAP"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			/* ° FIX SOMEDAY TO CHECK FOR VALID FONT MAPPING FILE PATHNAME. */

			strncpy(whichbuf+found->bc,source.resval.rstring,found->nc-1);

		} else if (!db_compareNames(found->name,"MENUNAME"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			/* ° FIX SOMEDAY TO CHOP THE EXTENSION AND FIND AND */
			/*	   LOAD THE MENU. */

			strncpy(fs1,source.resval.rstring,fs1sz_1); ic_trim(fs1,"be"/*DNT*/);
			if ((fshort1=strlen(fs1))>58) { msgn=SETVAR_STRING_TOO_LONG; goto out; }
			fi1=found->nc-1;  /* # of chars part 1 can hold (without the 0) */
			strncpy(whichbuf+found->bc,fs1,fi1);
			fi2=508;  /* Where part 2 starts. */
			if (fshort1>fi1) strncpy(whichbuf+fi2,fs1+fi1,44);
			else whichbuf[fi2]=0;
		} else if (!db_compareNames(found->name,"MTEXTED"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			// EBATECH(CNBR) -[ 2002/6/10 MTEXTED is NULL or Executable file
			if(strlen(source.resval.rstring)>0){
				char	fin[_MAX_PATH], fout[_MAX_PATH];
				strcpy(fin,source.resval.rstring);
				ic_setext(fin,"exe");
				if( sds_findfile(fin,fout) != RTNORM ){
					msgn = SETVAR_VALUE_OUT_OF_RANGE; goto out;
				}
			}
			// EBATECH(CNBR) ]-
			strncpy(whichbuf+found->bc,source.resval.rstring,found->nc-1);
		// EBATECH(CNBR) -[ 2002/6/28 Bugzilla#78220 SAVEFILEPATH is NULL or existing path.
		} else if (!db_compareNames(found->name,"SAVEFILEPATH"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if(strlen(source.resval.rstring)>0){
				if( _access(source.resval.rstring, 0 ) != 0 ){
					msgn = SETVAR_VALUE_OUT_OF_RANGE; goto out;
				}
			}
			strncpy(whichbuf+found->bc,source.resval.rstring,found->nc-1);
		// EBATECH(CNBR) ]-
		} else if (!db_compareNames(found->name,"SURFTYPE"/*DNT*/)) {
			if (found->type!=RTSHORT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if (source.resval.rint!=5 && source.resval.rint!=6 &&
				source.resval.rint!=8) { msgn=SETVAR_VALUE_OUT_OF_RANGE; goto out; }
			memcpy(whichbuf+found->bc,&source.resval.rint,shsz);
		} else if (!db_compareNames(found->name,"TARGET"/*DNT*/)	||
				   !db_compareNames(found->name,"LASTPOINT"/*DNT*/) ||
				   !db_compareNames(found->name,"LASTCMDPT"/*DNT*/) ||
				   !db_compareNames(found->name,"INSBASE"/*DNT*/)	||
				   !db_compareNames(found->name,"P_INSBASE"/*DNT*/)) {

			ic_ptcpy(ap1,source.resval.rpoint);
			if (!asis) {
				if (dp==NULL) { msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
				dp->trans(ap1,&ucsrb,&wcsrb,0,ap1,NULL);
			}
			memcpy(whichbuf+found->bc		   ,ap1  ,arsz);
			memcpy(whichbuf+found->bc+arsz	   ,ap1+1,arsz);
			memcpy(whichbuf+found->bc+arsz+arsz,ap1+2,arsz);
		} else if (strnisame(found->name,"TD",2)) {
			if (found->type!=RTREAL) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			ar2=modf(source.resval.rreal,&ar1);
			fl1=(long)ar1;
			memcpy(whichbuf+found->bc,&fl1,lsz);
			fl1=(long)(ar2*86400000.0);
			memcpy(whichbuf+found->bc+lsz,&fl1,lsz);
			/* If it's TDUSRTIMER, set dp->usrstarttime, too: */
			if (!db_compareNames(found->name,"TDUSRTIMER"/*DNT*/) && dp!=NULL)
				dp->set_usrstarttime();
		} else if (!db_compareNames(found->name,"TEXTSTYLE"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if (dp==NULL)			{ msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
			if ((thip1=dp->findtabrec(DB_STYLETAB,source.resval.rstring,1))==NULL)
				{ msgn=SETVAR_TABLE_RECORD_NOT_FOUND; goto out; }
			dp->current.textstyle=thip1;
		} else if (!db_compareNames(found->name,"UCSFOLLOW"/*DNT*/)) {
			if (found->type!=RTSHORT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			/* Get viewmode and change bit 3: */
			memcpy(&fushort1,whichbuf+found->bc,shsz);
			if (source.resval.rint) fushort1|=8; else fushort1&=~8;
			memcpy(whichbuf+found->bc,&fushort1,shsz);
		} else if (!db_compareNames(found->name,"UCSNAME"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if (dp==NULL)			{ msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
			if (!*source.resval.rstring) dp->current.ucsname=NULL;
			else {
				if ((thip1=dp->findtabrec(DB_UCSTAB,source.resval.rstring,1))==NULL)
					{ msgn=13; goto out; }
				dp->current.ucsname=thip1;
			}
		} else if (!db_compareNames(found->name,"P_UCSNAME"/*DNT*/)) {
			if (found->type!=RTSTR) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			if (dp==NULL)			{ msgn=SETVAR_DRAWING_POINTER_IS_NULL; goto out; }
			if (!*source.resval.rstring) dp->current.p_ucsname=NULL;
			else {
				if ((thip1=dp->findtabrec(DB_UCSTAB,source.resval.rstring,1))==NULL)
					{ msgn=SETVAR_TABLE_RECORD_NOT_FOUND; goto out; }
				dp->current.p_ucsname=thip1;
			}
		} else if (!db_compareNames(found->name,"USRTIMER"/*DNT*/)) {
			if (found->type!=RTSHORT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }

			/* Get the current value: */
			memcpy(&fshort1,whichbuf+found->bc,shsz);

			/* Only act if we're CHANGING its state: */
			if		(!fshort1 &&  source.resval.rint) {  /* Toggling it ON	*/
				if (dp!=NULL) dp->set_usrstarttime();
				fshort1=1; memcpy(whichbuf+found->bc,&fshort1,shsz);
			} else if ( fshort1 && !source.resval.rint) {  /* Toggling it OFF */
				if (dp!=NULL) {
					struct resbuf trb;

					/*
					**	All we have to do is getvar TDUSRTIMER and use
					**	that value to setvar it.  Sounds too easy to be
					**	true, but it's correct.  Since the timer is still
					**	on at this point, getvar will read the buffer
					**	and add the elapsed time since the timer was
					**	toggled ON.  That's the value we want to setvar.
					**	(The setvar is recursion, but that should be okay.)
					*/

					db_getvar(NULL,DB_QTDUSRTIMER,&trb,dp,configsv,sessionsv);
					db_setvar(NULL,DB_QTDUSRTIMER,&trb,dp,configsv,sessionsv,0);
				}
				fshort1=0; memcpy(whichbuf+found->bc,&fshort1,shsz);
			}

		} else if (!db_compareNames(found->name,"VSMAX"/*DNT*/)) {
			if (found->type!=RT3DPOINT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			/* ° MAY NOT BE INDEPENDENT. */
			/*	   Z MAY ALWAYS BE THE ELEVATION. */
			memcpy(whichbuf+found->bc,
				source.resval.rpoint,arsz);
			memcpy(whichbuf+found->bc+arsz,
				source.resval.rpoint+1,arsz);
			memcpy(whichbuf+found->bc+arsz+arsz,
				source.resval.rpoint+2,arsz);
		} else if (!db_compareNames(found->name,"VSMIN"/*DNT*/)) {
			if (found->type!=RT3DPOINT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			/* ° MAY NOT BE INDEPENDENT. */
			/*	   Z MAY ALWAYS BE THE ELEVATION. */
			memcpy(whichbuf+found->bc,
				source.resval.rpoint,arsz);
			memcpy(whichbuf+found->bc+arsz,
				source.resval.rpoint+1,arsz);
			memcpy(whichbuf+found->bc+arsz+arsz,
				source.resval.rpoint+2,arsz);
		} else if (!db_compareNames(found->name,"WORLDUCS"/*DNT*/)) {
			if (found->type!=RTSHORT) { msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out; }
			/* ° MAY NOT BE INDEPENDENT. */
			fshort1=(source.resval.rint!=0);
			memcpy(whichbuf+found->bc,&fshort1,shsz);
		// EBATECH(CNBR)
		} else {
			msgn=SETVAR_SPECIAL_CASE_CASE_NOT_FOUND; goto out;
		}

	} else {

		/* No special problems; stored in buffer in format expected */
		/* for this ->type. */

		switch (found->type) {
			case RTSHORT:
				memcpy(whichbuf+found->bc,&source.resval.rint,shsz);
				break;
			case RTREAL: case RTANG: case RTORINT:
				memcpy(whichbuf+found->bc,&source.resval.rreal,arsz);
				break;
			case RTSTR:
				if (found->nc>1) strncpy(whichbuf+found->bc,
					source.resval.rstring,found->nc-1);
				break;
			case RTPOINT: case RT3DPOINT:
				memcpy(whichbuf+found->bc,
					source.resval.rpoint,arsz);
				memcpy(whichbuf+found->bc+arsz,
					source.resval.rpoint+1,arsz);
				if (found->type==RT3DPOINT)
					memcpy(whichbuf+found->bc+arsz+arsz,
						source.resval.rpoint+2,arsz);
				break;
			case RTLONG:
				memcpy(whichbuf+found->bc,&source.resval.rlong,lsz);
				break;
			default:
				msgn=SETVAR_BAD_TYPE_IN_DATABASE; goto out;
		}
	}

	// EBATECH(watanabe)-[dimfit
	SetOtherVars(vname, qidx, dp, whichbuf, found, source.resval.rint);
	// ]-EBATECH(watanabe)

out:

	#if defined(DEBUG)
		if (msgn>SETVAR_NO_ERROR && msgn<sizeof(db_getsetvarmess)/sizeof(db_getsetvarmess[0])) {
			char *namep,fs1[161];

			if (vname!=NULL && *vname)		   namep=(char *)vname;
			else if (qidx>-1 && qidx<db_nvars) namep=db_oldsysvar[qidx].name;
			else							   namep=db_str_invalid;

			sprintf(fs1,"Setvar \"%s\"(%d): %s.\n"/*DNT*/,namep,qidx,db_getsetvarmess[msgn]);
			if (db_errormessfnp!=NULL) (*db_errormessfnp)(fs1,1);

		}
	#endif

	if		(msgn>SETVAR_TRUE_READ_ONLY && msgn<SETVAR_SPECIAL_CASE_CASE_NOT_FOUND && msgn!=SETVAR_DRAWING_POINTER_IS_NULL)
		db_lasterror=OL_ERANGE;
	else if (msgn==SETVAR_TRUE_READ_ONLY)						db_lasterror=OL_ERDONLY;

	return (msgn>SETVAR_NO_ERROR) ? RTERROR : RTNORM;
}



/*F*/
int db_compsysvar(const void *vp1, const void *vp2) {

	/* Use strcmp() -- not stricmp() -- so that we can sort */
	/* db_oldsysvar[] with QEDIT in db.h. */

	return strcmp(((db_sysvarlink *)vp1)->name,
				  ((db_sysvarlink *)vp2)->name);
}

/*F*/
db_sysvarlink *db_findsysvar(
	const char	*svname,
	int 		 qidx,
	db_drawing	*dp) {
/*
**	Finds an element in db_oldsysvar[] and returns a pointer to it,
**	or NULL, if not found.
**
**	If svname is non-NULL and non-"", it is used.  Otherwise, qidx
**	is used (if it is the defined range of DB_Q... values).
**
**	When dp is non-NULL, this function checks for PSPACE mode and
**	uses the "P_" versions.  (See the "P_" vars in db_oldsysvar[].)
**
**	Note that the return value may be for a different sysvar
**	than originally indicated by svname or qidx.
*/
	char fs1[IC_ACADNMLEN];
	bool	inps = false;
	bool	redirected = false;
	bool	named = false;
	db_sysvarlink key,*found;


// make sure one or the other of the inputs is valid
	if ((svname==NULL || !svname[0]) && (qidx<=-1 || qidx>=db_nvars)) return(NULL);

	found=NULL; inps=(dp!=NULL) ? dp->inpspace() : 0;

	if (svname!=NULL && *svname) {
		named = true;
		strncpy(fs1,svname,sizeof(fs1)-1); fs1[sizeof(fs1)-1]=0;
		strupr(fs1);  /* Comparison function uses strcmp(). */
		key.name=fs1;
		if (strsame(fs1,"LASTPT3D"/*DNT*/)) key.name="LASTPOINT"/*DNT*/;
		else if (inps) {
			redirected = true;

			if		(strsame(fs1,"ELEVATION"/*DNT*/))
				key.name="P_ELEVATION"/*DNT*/;
			else if (strsame(fs1,"EXTMAX"	/*DNT*/))
				key.name="P_EXTMAX"/*DNT*/	 ;
			else if (strsame(fs1,"EXTMIN"	/*DNT*/))
				key.name="P_EXTMIN"/*DNT*/	 ;
			else if (strsame(fs1,"INSBASE"	/*DNT*/))
				key.name="P_INSBASE"/*DNT*/  ;
			else if (strsame(fs1,"LIMCHECK" /*DNT*/))
				key.name="P_LIMCHECK"/*DNT*/ ;
			else if (strsame(fs1,"LIMMAX"	/*DNT*/))
				key.name="P_LIMMAX"/*DNT*/	 ;
			else if (strsame(fs1,"LIMMIN"	/*DNT*/))
				key.name="P_LIMMIN"/*DNT*/	 ;
			else if (strsame(fs1,"UCSNAME"	/*DNT*/))
				key.name="P_UCSNAME"/*DNT*/  ;
			else if (strsame(fs1,"UCSORG"	/*DNT*/))
				key.name="P_UCSORG"/*DNT*/	 ;
			else if (strsame(fs1,"UCSXDIR"	/*DNT*/))
				key.name="P_UCSXDIR"/*DNT*/  ;
			else if (strsame(fs1,"UCSYDIR"	/*DNT*/))
				key.name="P_UCSYDIR"/*DNT*/  ;
		}
		found=(db_sysvarlink *)bsearch(&key,db_oldsysvar,db_nvars,
			sizeof(db_sysvarlink),db_compsysvar);
	} else if (qidx>-1 && qidx<db_nvars) {
		if (inps) {
			redirected = true;

			if		(qidx==DB_QELEVATION)
				qidx=DB_QP_ELEVATION;
			else if (qidx==DB_QEXTMAX	)
				qidx=DB_QP_EXTMAX	;
			else if (qidx==DB_QEXTMIN	)
				qidx=DB_QP_EXTMIN	;
			else if (qidx==DB_QINSBASE	)
				qidx=DB_QP_INSBASE	;
			else if (qidx==DB_QLIMCHECK )
				qidx=DB_QP_LIMCHECK ;
			else if (qidx==DB_QLIMMAX	)
				qidx=DB_QP_LIMMAX	;
			else if (qidx==DB_QLIMMIN	)
				qidx=DB_QP_LIMMIN	;
			else if (qidx==DB_QUCSNAME	)
				qidx=DB_QP_UCSNAME	;
			else if (qidx==DB_QUCSORG	)
				qidx=DB_QP_UCSORG	;
			else if (qidx==DB_QUCSXDIR	)
				qidx=DB_QP_UCSXDIR	;
			else if (qidx==DB_QUCSYDIR	)
				qidx=DB_QP_UCSYDIR	;
			else
				redirected = false;
		}
		found=db_oldsysvar+qidx;
	}
		// Don't return paperspace variables unless redirected
	else if ( (found->spl & IC_SYSVARFLAG_PAPERSPACE) && named && !redirected )
		return NULL;

	return found;
}



/*F*/
DB_API int db_qgetvar(
	int   qidx,
	char *varbuf,
	void *res,
	int   dbvartype,
	int   strsz) {
/*
**	FOR USE BY PROGRAMMERS WHO KNOW HOW THE SYSTEM VARIABLE BUFFERS
**	WORK AND THE FORM OF THE DESIRED VALUE IN ITS BUFFER.  THIS IS AN
**	AID FOR THOSE PLACES YOU WOULD NORMALLY GO AND GET THE VARIABLE
**	VALUES OR DEFAULTS DIRECTLY (INSTEAD OF USING getvar()) USING THE
**	DB_Q... DEFINES AND memcpy().
**
**	WATCH OUT FOR VARIABLES THAT HAVE AN UNUSUAL FORM IN THE BUFFER.
**	EXAMPLE: TABLE REFERENCES LIKE CELTYPE.  db_oldsysvar[DB_QCELTYPE].type
**	IS RTSTR BECAUSE GETVAR/SETVAR TREAT IT AS A STRING -- BUT IN THE
**	BUFFER, IT'S A short TABLE INDEX.  SEE THE DESCRIPTION OF dbvartype
**	BELOW.
**
**	Nice things about this function:
**
**	  It automatically gives you the default value
**	  (db_sysvar[qidx].defval) if varbuf is NULL.
**
**	  It can fill any type of integer variable.
**
**	qidx : The DB_Q... code of the sysvar.
**
**	varbuf : The correct char buffer to extract the value from.
**			   (YOU have to know whether it's the header, configuration,
**			   or session buffer.)
**
**			   If NULL, the default will be used (db_oldsysvar[qidx].defval);
**			   this code knows that the source is a string in all cases.
**
**	res : The address of the result variable.  This MUST be a pointer
**			to a variable of the correct type for the dbvartype.
**			(See dbvartype below.):
**
**			dbvartype	What res must be
**			---------	--------------------------------------------------
**			  DB_CHAR : A (char *) (ptr to a single char).
**
**			 DB_SHORT : A (short *).
**
**			  DB_LONG : A (long *).
**
**			   DB_INT : An (int *).
**
**			DB_DOUBLE : An (sds_real *).
**
**		   DB_2DPOINT : An sds_point or an (sds_real *) pointing to
**						  an array of at least 2 sds_reals.
**
**		   DB_3DPOINT : An sds_point or an (sds_real *) pointing to
**						  an array of at least 3 sds_reals.
**
**			DB_STRING : A char pointer (char *) to an existing
**						  char buffer whose size is strsz.
**						  No deletion/allocation is performed --
**						  just a strncpy().
**
**		   DB_ASTRING : A POINTER to a char pointer (char **) for an
**						  allocated string.  The old value is deleted
**						  and the new one is allocated.
**
**	dbvartype : One of the DB_types discussed above.
**
**	strsz : Used in retrieving DB_STRING values.  Specifies the size
**			  of the char buffer that res points to.
**
**	Returns:
**		 0 : OK.
**		-1 : Invalid qidx.
**		-2 : dbvartype not handled.  Shouldn't happen.
**		-3 : db_oldsysvar[qidx].nc is inconsistent with dbvartype,
**			   so this function doesn't know how to read it.
**		-4 : Invalid variable to request (not in a buffer, or
**			   db_oldsysvar[qidx].bc or .nc are invalid).
*/
/*
**	Note for CPLOTSTYLE,DEFLPLSTYLE and DEFPLSTYLE :
**		User should check PSTYLEMODE before call db_qgetvar().
**		These value always "BYCOLOR" when PSTYEMODE==1(ByColor drawing).
**		db_getvar() checks PSTYLEMODE and set correct value.
*/
	char *fcp1;
	short fshort1;
	int fi1,rc=0;
	db_sysvarlink *var;


	if (qidx<0 || qidx>=db_nvars) { rc=-1; goto out; }

	var=db_oldsysvar+qidx;

	/* Weed out "special source" (no-buffer) vars and unusable locators: */
	if (var->spl & IC_SYSVARFLAG_SPECIALSOURCE || var->bc<0 || var->nc<1) { rc=-4; goto out; }

	if (varbuf==NULL) {  /* Use the sysvar default values (var->defval). */
		switch (dbvartype) {
			case DB_CHAR:	*((char 	*)res)= (char)atoi(var->defval); break;
			case DB_SHORT:	*((short	*)res)=(short)atoi(var->defval); break;
			case DB_INT:	*((int		*)res)= 	  atoi(var->defval); break;
			case DB_LONG:	*((long 	*)res)= 	  atol(var->defval); break;
			case DB_DOUBLE: *((sds_real *)res)= 	  atof(var->defval); break;
			case DB_2DPOINT:
				((sds_real *)res)[1]=0.0;
				fcp1=var->defval;
				((sds_real *)res)[0]=atof(fcp1);
				while (*fcp1 && *fcp1!=',') fcp1++;
				if (*fcp1) ((sds_real *)res)[1]=atof(++fcp1);
				break;
			case DB_3DPOINT:
				((sds_real *)res)[1]=((sds_real *)res)[2]=0.0;
				fcp1=var->defval;
				((sds_real *)res)[0]=atof(fcp1);
				while (*fcp1 && *fcp1!=',') fcp1++;
				if (*fcp1) {
					((sds_real *)res)[1]=atof(++fcp1);
					while (*fcp1 && *fcp1!=',') fcp1++;
					if (*fcp1) ((sds_real *)res)[2]=atof(++fcp1);
				}
				break;
			case DB_STRING:
				if (strsz<1) *((char *)res)=0;
				else {
					strncpy((char *)res,var->defval,strsz-1);
					((char *)res)[strsz-1]=0;
				}
				break;
			case DB_ASTRING:
				db_astrncpy((char **)res,var->defval,-1);
				break;
			default: rc=-2; break;
		}
	} else {  /* Use the char buffer to get current values. */
		switch (dbvartype) {
			case DB_CHAR: case DB_SHORT: case DB_INT: case DB_LONG:
				if		(var->nc==sizeof(char)) {
					fi1=varbuf[var->bc];
				} else if (var->nc==sizeof(short)) {
					memcpy(&fshort1,varbuf+var->bc,var->nc);
					fi1=fshort1;
				} else if (var->nc>=sizeof(int)) {
					memcpy(&fi1,varbuf+var->bc,var->nc);
				} else { rc=-3; goto out; }
				switch (dbvartype) {
					case DB_CHAR:  *((char	*)res)= (char)fi1; break;
					case DB_SHORT: *((short *)res)=(short)fi1; break;
					case DB_INT:   *((int	*)res)= 	  fi1; break;
					case DB_LONG:  *((long	*)res)= (long)fi1; break;
				}
				break;
			case DB_DOUBLE:
				if (var->nc>=sizeof(sds_real))
					memcpy(res,varbuf+var->bc,var->nc);
				else rc=-3;
				break;
			case DB_3DPOINT:
				fi1=sizeof(sds_real);
				if (var->nc>=3*fi1) {
					memcpy((char *)res,varbuf+var->bc,fi1);
					memcpy((char *)res+fi1,varbuf+var->bc+fi1,fi1);
					memcpy((char *)res+fi1+fi1,varbuf+var->bc+fi1+fi1,fi1);
				} else rc=-3;
				break;
			case DB_2DPOINT:
				fi1=sizeof(sds_real);
				if (var->nc>=2*fi1) {
					memcpy((char *)res,varbuf+var->bc,fi1);
					memcpy((char *)res+fi1,varbuf+var->bc+fi1,fi1);
					// EBATECH(CNBR) 06-01-2002 -[ preserve to set abnormal Z value.
					if (var->nc>=3*fi1) {
						*((sds_real*)res+2)= 0.0;
					}
					// EBATECH(CNBR) ]-
				} else rc=-3;
				break;
			case DB_STRING:
				if (strsz<1) *((char *)res)=0;
				else {
					if ((fi1=strsz-1)>var->nc) fi1=var->nc;
					strncpy((char *)res,varbuf+var->bc,fi1);
					((char *)res)[fi1]=0;
				}
				break;
			case DB_ASTRING:
				db_astrncpy((char **)res,varbuf+var->bc,var->nc);
				break;
			default: rc=-2; break;
		}
	}


out:
	return rc;
}

#if defined(DEBUG)

	/*F*/
	void db_seterrormessfnp(void (*fnp)(char *mess, int wherefrom)) {
	/*
	**	Called to give a pointer to a function for reporting
	**	error messages.  This was done specifically for getvar and setvar,
	**	but any other DB functions can use it, too.
	*/
		db_errormessfnp=fnp;
	}

#endif

DB_API struct db_sysvarlink *db_metric_sysvarlink(void) { return(db_measurement_metric); }

DB_API int db_getvaralt(
	int   qidx,
	db_sysvarlink* altbuf,
	char** varname,
	resbuf *res) {
/*
**	Get system variable default value from alternate buffer,
**	db_measurement_metric or db_pstylemode_named.
**
**	qidx	: (i) 0-n index of alternate sysvarlink
**	altbuf	: (i) pointer of alternate sysvarlink
**	varname : (o) variable name (Don't free)
**	res 	: (o) The address of result buffer(Should free later)
**
**	Returns:
**		RTNORM	: OK.
**		RTERROR : NG
*/
	char *fcp1;
	int rc=RTNORM;
	db_sysvarlink *var;


	if (qidx < 0 || varname == NULL )
	{ rc=RTERROR;}
	else
	{
		var=altbuf+qidx;
		res->resval.rstring=NULL;
		res->restype=var->type;
		*varname = var->name;
		/* Use the sysvar default values (var->defval). */
		switch (var->type) {
		case RTSHORT:
			res->resval.rint = (short)atoi(var->defval);
			break;
		case RTREAL: case RTANG: case RTORINT:
			res->resval.rreal = atof(var->defval);
			break;
		case RTSTR:
			db_astrncpy((char**)&res->resval.rstring,var->defval,-1);
			break;
		case RTPOINT: case RT3DPOINT:
			res->resval.rpoint[0] = res->resval.rpoint[1] = res->resval.rpoint[2] = 0.0;
			fcp1=var->defval;
			res->resval.rpoint[0]=atof(fcp1);
			while (*fcp1 && *fcp1!=',') fcp1++;
			if (*fcp1) {
				res->resval.rpoint[1]=atof(++fcp1);
				while (*fcp1 && *fcp1!=',') fcp1++;
				if (*fcp1) res->resval.rpoint[2]=atof(++fcp1);
			}
			break;
		case RTLONG:
			res->resval.rlong = atol(var->defval);
			break;
		default:
			res->restype=RTNIL;
			rc =RTERROR;
		}
	}
	return rc;
}
