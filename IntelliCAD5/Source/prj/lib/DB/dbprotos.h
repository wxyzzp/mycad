/* File  : <DevDir>\source\prj\lib\db\dbprotos.h
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 */

#ifndef _INC_DBPROTOS
#define _INC_DBPROTOS

DB_API short	db_getendpts(sds_name ename, sds_point pt[2]);
DB_API short	db_edata(sds_name ename, short* closedp, short* nvertsp, sds_real *lenp, sds_real* areap);
DB_API int		db_is_entitytype(int typ);
DB_API int		db_is_subentitytype(int typ);
DB_API int		db_is_tabrectype(int typ);
DB_API int		db_is_objecttype(int typ);
DB_API int		db_is_validhitype(int typ);
DB_API int		db_chkrange(const resbuf* rbp, char* range);
DB_API int		db_comphand(const char* hand1, const char* hand2);
DB_API int		db_convhand(char* sour, char* dest, int sform, int dform);
db_sysvarlink*	db_findsysvar(const char* svname, int qidx, db_drawing* dp);
void			db_getucs(sds_point org, sds_point xdir, sds_point ydir, sds_point zdir, db_drawing* dp);

typedef bool (*pfnSerialNumber)(LPTSTR number, DWORD size);
DB_API bool		db_SetSerialNumber(pfnSerialNumber func);

int				db_getvar(const char* vname, int qidx, resbuf* res, db_drawing* dp, db_charbuflink* configsv, db_charbuflink* sessionsv);
DB_API char*	db_hitype2str(int hitype);
char*			db_makeanonname(char* sour, db_drawing* dp, int donotdisturb);
db_handitem*	db_makehip(int hiptype, db_drawing* dp, char* tabrecname);
//create proper handitem and assign values from resbuf.
DB_API db_handitem*	db_makehipfromrb(resbuf* rb, db_drawing* drawing);
int				db_makesvbuf(int type, db_charbuflink* svbufp, db_drawing* dp);
DB_API resbuf*	db_alloc_resbuf();
DB_API void		db_free_resbuf(resbuf* rb);
resbuf*			db_newrb(short rtype, int valtype, void* valp);
DB_API resbuf*	db_alloc_resbuf_list(size_t n);
DB_API void		db_free_resbuf_list(resbuf* rb);
DB_API int		db_qgetvar(int qidx, char* varbuf, void* res, int restype, int strsz);
DB_API int		db_ret_lasterror();
int				db_setvar(const char* vname, int qidx, const resbuf* argsour, db_drawing* dp, db_charbuflink* configsv, db_charbuflink* sessionsv, int asis);
DB_API int		db_str2hitype(char* typestr);
DB_API int		db_str2rb(char* str, resbuf* rbp);
DB_API char*	db_sysvarqidx2str(int svidx);
int				db_tab2tabrecfn(int tabtype);
DB_API int		db_tabrec2tabfn(int hitype);
DB_API int		db_unitizevector(sds_point vect);
DB_API void		db_setgrafunptrs(void (*grafreedofp)(void*));
DB_API db_sysvarlink*	db_GetLastSetvar();
DB_API db_sysvarlink*	db_Getsysvar();

DB_API short	db_Getnvars();
DB_API void		db_setundofunptr(void (*undofreefp)(void*));
DB_API void		db_AsciiHexToBinary(char* readbuf, char* storebuf, int storebufflen, short* validchars);
DB_API void		db_BinaryHexToAscii(unsigned char* bindata, short bindatalength, char* tempstr, int tempstrlen);

///***** PROTOS NEEDED BY IN-LINE MEMBER FUNCTIONS ************
DB_API int	db_aextrucpy(sds_real **extrup, sds_point newextru);
DB_API void db_astrncpy(char** cpp, const char* newval, int maxnc);
DB_API void db_mstrncpy(char** cpp, const char* newval, int maxnc);

// Helper functions for DIMENSIONs:
DB_API int		db_ucshelper(sds_point ucsorg, sds_point* ucsaxis, sds_real elev, sds_point ecsinspt, sds_point wcsinspt, sds_real* protp);
DB_API void		db_initdimucs(sds_point* dimucs, sds_real* dimelevp, db_drawing* dp, db_handitem* dimhip);
DB_API int		db_samedirection(sds_point v0, sds_point v1);
DB_API int		db_setfont(char* fontname, char* foundpn, db_fontlink** fontpp, db_drawing* dp);
void			db_getfontspacing(db_fontlink* font, sds_real* lffactp, sds_real* spfactp);
short			db_getfontval(char** lpp);
DB_API int		db_setfontmap(char* pn);
DB_API void		db_freefontmap();
DB_API int		db_setfontusingmap(char* fontname, char* foundpn, db_fontlink** fontpp, db_fontsubstmode fontsubstmode, db_drawing* dp);
DB_API char*	db_usefontmap(char* oldpn);
DB_API void		db_setfontsubstfn(void (*fn)(char* looked4, char* found));

/*DG - 11.6.2002*/// This function is needed at least for layer's dxf370 setting which is supported since dxf 2000.
DB_API bool db_is_validweight(int weight, int bitcode );
DB_API int db_lweight2index( int weight );
DB_API int db_index2lweight(int index );

DB_API int db_getvaralt(int qidx, db_sysvarlink* altbuf, char** varname, resbuf *res);
DB_API db_sysvarlink* db_metric_sysvarlink(void);
DB_API db_handitem* db_ret_default_plotstylehip(db_drawing* dp );
DB_API bool db_current_plotstyle( db_drawing* dp, int* plotstyletype, db_handitem** retp );
DB_API char* db_ret_plotstylename( db_handitem* hip, db_drawing* dp );

/******************************************************************************************
* Author:		Denis Petrov
* Description:	This functions try to find code/name of symbol	in font - pFont by his name/code - pName.
******************************************************************************************/
DB_API int		db_findCodeByName(const db_fontlink* pFont, const char* pName);
DB_API int		db_findNameByCode(char* pName, int code, const db_fontlink* pFont);

#ifdef DEBUG
void			db_seterrormessfnp(void (*fnp)(char* mess, int wherefrom));
#endif

// NOTE:  Code copied from CRT source.
// We needed a version that didn't look at locale, which stricmp does.
inline int __cdecl db_compareNames(const char* dst, const char* src)
{
	int	f, l;

	do
	{
		if(((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z'))
			f -= 'A' - 'a';

		if(((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z'))
			l -= 'A' - 'a';
	} while(f && f == l);

	return f - l;
}


#endif // _INC_DBPROTOS
