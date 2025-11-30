/* F:\DEV\PRJ\LIB\SDS\SDS_FINDFILE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

//*********************************************************************
//*  sds_findfile.CPP                                                 *
//*  Copyright (C) 1997, 1998 by Visio Corporation                    *
//*                                                                   *
//*********************************************************************
//** Misc Functions that can all be stand alone 
//*********************************************************************

//** Includes

#include "db.h"/*DNT*/
#include "icadlib.h"/*DNT*/
#include "sds.h"/*DNT*/
#include <tchar.h>
#include <string>

using std::string;

#ifndef	SDS_PROGRAMFILE
	#define	SDS_PROGRAMFILE	"icad.exe"/*DNT*/
#endif

#include <direct.h>
//** Functions

static void 
constructpathandfname( const char *origpath, const char *origname, char *newpath, char *newname)
	{
	char pathbuf[IC_ACADBUF],drive[5],ext[IC_PATHSIZE],dir[IC_PATHSIZE];

// concatenate the path and the filename, since filename can itself be a relative path,
// then resplit into path and filename
// 5/2/99 -- if origname is a FULL path, and there is an origpath, then use only the filename portion of origname
	if (strlen(origpath) == 0)
	{
		pathbuf[0] = '\\';
		pathbuf[1] = '\0';
	} else
	{
		strcpy(pathbuf,origpath);
		if (pathbuf[strlen(pathbuf)-1]!='\\')
			strcat(pathbuf,"\\");
	}
// splitpath on the origname to see if it is a full path spec (has either drive or \ at start of path)
    _splitpath(origname,drive,dir,newname,ext);
	if (pathbuf[0] && (drive[0] || pathbuf[0]=='\\')) // origname is full path, origpath is not NULL
		{
		strcat(pathbuf,newname);	// use only the filename portion
		strcat(pathbuf,ext);
		}
	else
		{
		strcat(pathbuf,origname);	// it was a relative path or filename only
		}
	_splitpath(pathbuf,drive,dir,newname,ext);
	sprintf(newpath,"%s%s",drive,dir);
	strcat(newname,ext);
	}

// Modified PK 09/08/2000 [
void RemoveBackSlash(char str[])  // Reason : Fix to bug no. 7654
{
	int i,j;
	//EBATECH(Futagami) MBCS String [
	//for(i=0,j=0; str[i]!='\0'; i++)
	//	if (str[i] != '\\')
	//		str[j++] = str[i];
	for(i=0,j=0; str[i]!='\0'; i++){
		if ((_MBC_LEAD ==_mbbtype((unsigned char)str[i],0)) &&
		   (_MBC_TRAIL==_mbbtype((unsigned char)str[i+1],1)) ){
			str[j++] = str[i++];
			str[j++] = str[i];
			continue;
		}
		if (str[i] != '\\')
			str[j++] = str[i];
	}
	//EBATECH ]
	str[j] = '\0';
}

int FindUserPathInCompletePath(char *PathFound, char *UserPath)
{
	char *str1, *str2;
	str1 = str2 = NULL;
//	str1 = strdup(PathFound);
//	str2 = strdup(UserPath);
	db_astrncpy(&str1, PathFound, -1);
	db_astrncpy(&str2, UserPath, -1);
	RemoveBackSlash(str1);
	RemoveBackSlash(str2);
	strupr(str1);
	strupr(str2);
	string string1=str1, string2=str2;
	IC_FREE(str1);
	IC_FREE(str2);
	if(string1.find(string2) == string::npos)
		return 0;
	return 1;
}
// Modified PK 09/08/2000 ]

int
ic_findinpath( LPCTSTR specificPath, const char *szLookFor, char *szPathFound, DWORD bufSize)
	{
    char *fcp1,*lookfor;
	struct resbuf rb;
	TCHAR searchpathbuf[IC_ACADBUF],fname[IC_ACADBUF];

	if(szLookFor==NULL || *szLookFor==0 || szPathFound ==NULL) return(RTERROR);

	lookfor= new char [strlen(szLookFor)+1];
	strcpy(lookfor,szLookFor);

	ChangeSlashToBackslashAndRemovePipe(lookfor);

	if(0==access(lookfor,00)) {
		if(!strchr(lookfor,':'/*DNT*/) 
			// DP: need to check for network path
			&& lookfor[0] != '\\'/*DNT*/ && lookfor[1] != '\\'/*DNT*/) {
            // GetCurrentDirectory() will fail on Windows 95 at 
            // unexpected times. I have added a call to getcwd()
            // if it does fail. On occation it fails with GetLastError() of 87
            // 87 The parameter is incorrect.   ERROR_INVALID_PARAMETER 
            // Whatever that means. I tried all the tricks I could
            // think of. (Hitachi fix)
            if (GetCurrentDirectory(bufSize, szPathFound) == 0)
                char* results = getcwd(szPathFound, bufSize);
			strcat(szPathFound,"\\"/*DNT*/);
		} else *szPathFound=0;
		strcat(szPathFound,lookfor);
		IC_FREE(lookfor);
		return(RTNORM);
	}

    if(0<SearchPath(NULL,lookfor,NULL,bufSize,szPathFound,&fcp1)) {
		IC_FREE(lookfor);
		return(RTNORM);
	}

	// Check specific search path
	if ( specificPath ) {
		constructpathandfname( specificPath,lookfor,searchpathbuf,fname);
		if(0<SearchPath(searchpathbuf,fname,NULL,bufSize,szPathFound,&fcp1)) {
			IC_FREE(lookfor);
			return(RTNORM);
		}
	}

	static int (*LibFunc)(const char *sym, struct sds_resbuf *szPathFound)=NULL;
	rb.resval.rstring=NULL;
#if defined(SDS_SDSSTANDALONE) || 1
	if ( !LibFunc )
		{
		HINSTANCE hInstLib=NULL;

		if((hInstLib=GetModuleHandle("icad.exe"/*DNT*/))==NULL) {
			IC_FREE(lookfor);
			return(RTERROR);
			}

		if((LibFunc=(int (*)(const char *sym, struct sds_resbuf *szPathFound))GetProcAddress(hInstLib,"sds_getvar"/*DNT*/))==NULL) {
			IC_FREE(lookfor);
			return(RTERROR);
			}
		}

	(*LibFunc)("SRCHPATH"/*DNT*/,&rb);
#else
	db_getvar(NULL,DB_QSRCHPATH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
#endif

	// Check generic search path
	constructpathandfname("",lookfor,searchpathbuf,fname);
	string	srchpathVar = rb.resval.rstring;
	if(rb.resval.rstring) {
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring=NULL;
	}

    if(0<SearchPath(srchpathVar.c_str(),fname,NULL,bufSize,szPathFound,&fcp1)) {
		// Modified PK 09/08/2000 [
		if (!FindUserPathInCompletePath(szPathFound, lookfor)) // Reason : Fix to bug no. 7654
		{
			IC_FREE(lookfor);
			return RTERROR;
		}
		// Modified PK 09/08/2000 ]
		IC_FREE(lookfor);
		return(RTNORM);
	}
    // If we get this far and haven't found it yet, we might at well try the lookfor
    // form of what to find. This fixes bug 1-58947 where the bug says things like:
    // (findfile "lisp\\sample.lsp") is being looked for on the search path. 
    // lookfor is 'lisp\\sample.lsp' 'relative to a searchpath path. So if c:\junk
    // was on the search path, and under that lisp\\sample.lsp exists, this will find it
    // because it would be searched for as is instead of how it is above using fname, which
    // is simply 'sample.lsp'. 
    if(0<SearchPath(srchpathVar.c_str(),lookfor,NULL,bufSize,szPathFound,&fcp1)) {
		IC_FREE(lookfor);
		return(RTNORM);
	}

	
	// Chech the drawing file dir.
#if defined(SDS_SDSSTANDALONE) || 1
	{
		(*LibFunc)("DWGPREFIX"/*DNT*/,&rb);
	}
#else
	db_getvar(NULL,DB_QDWGPREFIX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
#endif

	constructpathandfname(rb.resval.rstring,lookfor,searchpathbuf,fname);
	if(rb.resval.rstring) {
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring=NULL;
	}


	if(0<SearchPath(searchpathbuf,fname,NULL,bufSize,szPathFound,&fcp1)) {
		IC_FREE(lookfor);
		return(RTNORM);
	}


	int sl=strlen(szLookFor);
	if(sl>4 && !strchr(szLookFor,'\\'/*DNT*/)) {
		static TCHAR appPath[IC_ACADBUF];
		static bool needAppPath = true;

		if ( needAppPath )
			{
			ic_findinpath(NULL, SDS_PROGRAMFILE,appPath, bufSize);
			needAppPath = false;
			}

		if(strisame(&szLookFor[sl-4],".shx"/*DNT*/) || strisame(&szLookFor[sl-4],".shp"/*DNT*/)) {
			if ( appPath[0] EQ ' ' )
				goto out;
			TCHAR icpath[IC_ACADBUF];
			_tcscpy( icpath, appPath);
			strcpy(&icpath[strlen(icpath)-strlen(SDS_PROGRAMFILE)],"Fonts\\"/*DNT*/);
			strcat(icpath,szLookFor);
			if(0!=access(icpath,00)) goto out;
			strncpy(szPathFound,icpath,bufSize);
			IC_FREE(lookfor);
			if(rb.resval.rstring) {
				IC_FREE(rb.resval.rstring);
				rb.resval.rstring=NULL;
			}
			return(RTNORM);
		}
		if(strisame(&szLookFor[sl-4],".hlp"/*DNT*/ )) {
			if ( appPath[0] EQ ' ' )
				goto out;
			TCHAR icpath[IC_ACADBUF];
			_tcscpy( icpath, appPath);
			strcpy(&icpath[strlen(icpath)-strlen(SDS_PROGRAMFILE)],"Help\\"/*DNT*/);
			strcat(icpath,szLookFor);
			if(0!=access(icpath,00)) goto out;
			strncpy(szPathFound,icpath,bufSize);
			IC_FREE(lookfor);
			if(rb.resval.rstring) {
				IC_FREE(rb.resval.rstring);
				rb.resval.rstring=NULL;
			}
			return(RTNORM);
		}
	}

	char *cp1;
	if(NULL!=(cp1=strrchr(szLookFor,'\\'/*DNT*/))) {
		IC_FREE(lookfor);
		if(rb.resval.rstring) {
			IC_FREE(rb.resval.rstring);
			rb.resval.rstring=NULL;
		}
		return(ic_findinpath(NULL, cp1+1,szPathFound, bufSize));			
	}

	out: ;

	IC_FREE(lookfor);
	if(rb.resval.rstring) {
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring=NULL;
	}
	return(RTERROR);
}

// *****************************************
// This is an SDS External API
//
// SPECIAL !!!
// Because of multiple dependencies, there is no sdsengine_ version
// of this function
//
int 
sds_findfile(const char *szLookFor, char *szPathFound) 
	{
	return ic_findinpath( NULL, szLookFor, szPathFound);
	}
    


