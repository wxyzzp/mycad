/* F:\ICADDEV\PRJ\LIB\ICADLIB\ICADLIB.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Common library service routines
 *
 */

#pragma once
#ifndef _ICADLIB_H
#define _ICADLIB_H

#define WIN32_LEAN_AND_MEAN	/*DG - 1.3.2002*/// exclude rarely used stuff from windows.h

#ifndef	_WINDOWS_
#include <windows.h>
#endif
#include <tchar.h>

#ifndef _ICADDEF_H
	#include "icaddef.h"
#endif

#ifndef _SDS_H
	#include "sds.h"
	#undef	T			// only used by SDS applications
	#undef	X
	#undef	Y
	#undef	Z
#endif

	// private sds functions
int sds_entnext_noxref( const sds_name nmKnownEnt, sds_name nmNextEnt);

#ifndef _INC_STDLIB
	#include <stdlib.h>
#endif
#ifndef _INC_STDIO
	#include <stdio.h>
#endif
#ifndef _INC_IO
	#include <io.h>
#endif
#ifndef _INC_CTYPE
	#include <ctype.h>
#endif
#ifndef _INC_MATH
	#include <math.h>
#endif
#ifndef _INC_MALLOC
	#include <malloc.h>
#endif
#ifndef _INC_MBSTRING
	#include <mbstring.h>
#endif
#ifndef _INC_MBCTYPE
	#include <mbctype.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#if defined(__cplusplus)
//	extern "C"
//	{
#endif

#ifndef	_TCHAR_DEFINED
typedef char			TCHAR;
typedef TCHAR			*LPTSTR;
typedef const TCHAR		*LPCTSTR;
#endif

typedef long			LPARAM;
typedef int				BOOL;

#if !defined(IC_OURSTRINGSTUFF)
	#define IC_FILECASE(str) strupr(str)
	#define IC_CHMOD(fn)   strlen(fn) /* this is just a do nothing function */
#else
	#define IC_FILECASE(str) ic_strupr(str)
	#define IC_CHMOD(fn)   ic_strlen(fn) /* this is just a do nothing function */
#endif

#define IC_SLASH	  '\\'
#define IC_SLASHSTR   "\\"
#define IC_OPSLASH	  '/'
#define IC_OPSLASHSTR "/"

#define IC_FILESIZE    MAX_PATH
#define IC_PATHSIZE    MAX_PATH
#define IC_MAXENVLN   131	/* DOS experimenting gives 123 */

#define IC_EXIST_OK  0
#define IC_READ_OK	 4
#define IC_WRITE_OK  2

#define IC_PATHPARS ";"

#define IC_DWGEXT	   ".dwg"
#define IC_DATEXT	   ".dat"
#define IC_BAKEXT	   ".bak"

#define IC_ZRO			 1.0E-10

// Modified CyberAge VSB 02/23/2001
// Reason: Fix for Bug No. 58859 from Bugzilla
//#define IC_ACADBUF	512   /* max acad buffer for getstring & tile	 */
#define IC_ACADBUF	  2049	 /* max acad buffer for getstring & tile(8*256+1)*/
#define IC_ACADNMLEN  256	 /* number of char in ACAD name 			*/
#define IC_SLBHEADER  32L	/* length of slide library header		   */
#define IC_SLBDIRHEAD 36L	/* length of slide library directory	   */
#define IC_XDLIMIT	  16384 /* max length of total eed				   */
//Bugzilla No. 78447 ; 18-03-2003 
#define IC_SNAPSTR	  75	/* max length for snap string				   */

#define IC_FREEIT  if(elist) { sds_relrb(elist); elist=NULL; }

#define ic_ptcpy(P0,P1) (P0)[0]=(P1)[0],(P0)[1]=(P1)[1],(P0)[2]=(P1)[2]
#define ic_encpy(E0,E1) (E0)[0]=(E1)[0],(E0)[1]=(E1)[1]


extern char   ic_um_majword[8], ic_um_majabv[3], ic_um_minabv[3], ic_um_temp[2], ic_um_weight[4];
extern double ic_um_convert;
extern char   ic_um_units;

extern char   ic_byblock[],ic_bylayer[],ic_bycolor[];

extern char   ic_genfname[];

extern struct resbuf *ic_rbassoc;	/* return buffer for ic_assoc function */

struct ic_pointsll {		/* Llist of 2D points */
	sds_point pt;
	struct ic_pointsll *next;
};

struct StyleItem	// needed across projects for Text Style dialogs and utilities
	{
	TCHAR styleName[LF_FACESIZE];
	TCHAR fullName[LF_FULLFACESIZE];	// localized font family name
	int weight;
	int italic;
	int pitchAndFamily;
	int charSet; 	// EBATECH(CNBR) Add charSet
	};

//#define sleep(i1) 		 ic_sleep(i1)

#if !defined(IC_OURSTRINGSTUFF)
	#ifndef _INC_STRING
		#include <string.h>
	#endif

	#define stristr(s1,s2)		ic_stristr(s1,s2)
	// Always use our strncpy because it plants a 0 in the string.
	#define strncpy(s1,s2,i1)  ic_strncpy( s1, s2, i1 )

	// Always use our strupr because it returns appropriately if s1 is NULL.  The system strupr crashes with
	// setlocale being something other that C because it does a strcmp with a NULL.
	#define strupr(s1)		   ic_strupr(s1)

#else
	void  *memchr(void *s, int c, size_t n);
	int    memcmp(const void *s1, const void *s2, size_t n);
	void  *memcpy(const void *s1, const void *s2, size_t n);
	void  *memmove(void *s1, const void *s2, size_t n);
	void  *memset(void *s, int c, size_t n);

	#define strstr(s1,s2)	   ic_strstr(s1,s2)
	#define strcat(s1,s2)	   ic_strcat(s1,s2)
	#define strchr(s1,i1)	   ic_strchr(s1,i1)
	#define strcmp(s1,s2)	   ic_strcmp(s1,s2)
	#define stricmp(s1,s2)	   ic_stricmp(s1,s2)
	#define strcpy(s1,s2)	   ic_strcpy(s1,s2)
	#define stristr(s1,s2)	   ic_stristr(s1,s2)
	#define strlen(ss)		   ic_strlen(ss)
	#define strlwr(ss)		   ic_strlwr(ss)
	#define strncat(s1,s2,i1)  ic_strncat(s1,s2,i1)
	#define strncpy(s1,s2,i1)  ic_strncpy(s1,s2,i1)
	#define strnicmp(s1,s2,i1) ic_strnicmp(s1,s2,i1)
	#define strrchr(s1,i1)	   ic_strrchr(s1,i1)
	#define strrev(ss)		   ic_strrev(ss)
	#define strpbrk(s1,s2)	   ic_strpbrk(s1,s2)
	#define strspn(s1,s2)	   ic_strspn(s1,s2)
	#define strtok(s1,c1)	   ic_strtok(s1,c1)
	#define strupr(s1)		   ic_strupr(s1)
	#define strnset(s1,i1,i2)  ic_strnset(s1,i1,i2)
	#define strset(s1,i1)	   ic_strset(s1,i1)
	#define strcspn(p1,p2)	   ic_strcspn(p1,p2)
	#if !defined(UNIX)
		#define strtod(s1,s2)	   ic_strtod(s1,s2)
		#define strtof(s1,s2)	   ic_strtof(s1,s2)
	#endif
#endif


//#if !defined(USE_SMARTHEAP) && defined(USE_CRTDBG)
//	#include <crtdbg.h>
//	#define _INC_MALLOC

//	#define new DEBUG_NEW
//	#define MALLOC(x)	 _malloc_dbg(x, _NORMAL_BLOCK, __FILE__, __LINE__)
//	#define CALLOC(n,x)  _calloc_dbg((n),(x), _NORMAL_BLOCK, __FILE__, __LINE__)
//	#define REALLOC(m,x) _realloc_dbg((m),(x), _NORMAL_BLOCK, __FILE__, __LINE__)
//	#define FREE(m) 	 _free_dbg(m,_NORMAL_BLOCK)
//	#define malloc(x)	 _malloc_dbg(x, _NORMAL_BLOCK, __FILE__, __LINE__)
//	#define calloc(n,x)  _calloc_dbg((n),(x), _NORMAL_BLOCK, __FILE__, __LINE__)
//	#define realloc(m,x) _realloc_dbg((m),(x), _NORMAL_BLOCK, __FILE__, __LINE__)
//	#define free(m) 	 _free_dbg(m,_NORMAL_BLOCK)
//#else
//	#define MALLOC(x)	 malloc(x)
//	#define CALLOC(n,x)  calloc((n),(x))
//	#define REALLOC(m,x) realloc((m),(x))
//	#define FREE(m) 	 free(m)
//#endif

#define IC_RELRB(x) if(x) { sds_relrb(x); x=NULL; }
//#define IC_FREE(x) if(x) { free(x); x=NULL; }
#define IC_FREE(x) if(x) { delete [] (x); x=NULL; }


/************** utility for immediate resbuf ****************/
struct AutoResbuf : resbuf
	{
	AutoResbuf( short value) { restype = RTSHORT; resval.rint = value;}
	AutoResbuf( long  value) { restype = RTLONG; resval.rlong = value;}
	AutoResbuf( sds_real value) { restype = RTREAL; resval.rreal = value;}
	AutoResbuf( sds_point value) { restype = RT3DPOINT;
									resval.rpoint[0] = value[0];
									resval.rpoint[1] = value[1];
									resval.rpoint[2] = value[2];
									}

	operator resbuf *() {return this;};
	};


	// abstract base class for holding pointer to unknown data
	// owner can delete without knowing full definition
class AbstractDelete
	{
public:
	virtual ~AbstractDelete() {};
	virtual void* TruePointer() = 0;	// leaf will {return this;} which can be cast
	};


#ifndef INTELLICAD
#define INTELLICAD // needed for the following Geo include statements
#endif
#include "Geometry.h"/*DNT*/
#include "Point.h"/*DNT*/
#include "Trnsform.h"/*DNT*/
#include "Plane.h"


class CMirrorPlane : public CTransform
	{

	public:

		// construct from the point and normal of the plane
		CMirrorPlane(C3Point P, C3Point N, RC & rc);

		RC ApplyToPoint(
			C3Point ptIn,			// In: The point to be transformed
			C3Point & ptOut) const;	// Out: the transformed point

		RC ApplyToVector(
			C3Point ptIn,			   // In: The vector to be transformed
			C3Point & ptOut) const;    // Out: The transformed vector


	private:

		CPlane m_Plane;
	};

/*DG - 31.10.2001*/// Keep this enum in synch with COM's IntersectOptions enum
// defined in icadtlb.h.
enum EInterExtendOption
{
    eExtendNone   = 0,
	eExtendFirst  = eExtendNone + 1,
	eExtendSecond = eExtendFirst + 1,
	eExtendBoth	  = eExtendSecond + 1
};

/************** Prototypes are all below ****************/
short		   ic_2pta2arc(sds_point p0, sds_point p1, sds_real tandir, sds_point cent, sds_real *radp, sds_real *sangp, sds_real *eangp);
int			   ic_3dxseg(sds_point s0p0, sds_point s0p1, short type0, sds_point extru0, sds_name ent0,
						sds_point s1p0, sds_point s1p1, short type1, sds_point extru1, sds_name ent1,
						sds_real fuzz, sds_point* &pXPoints);
short		   ic_3pt2arc(sds_point ep1, sds_point mp, sds_point ep2, sds_point cent, sds_real *rad, sds_real *sa, sds_real *ea);
short		   ic_acadver(short ver);
void		   ic_adscode(short code, char *str);
void		   ic_angpar(sds_real sour, sds_real *destp, sds_real dist1, sds_real dist2,short mode);
short		   ic_arbaxis(sds_point extrudir, sds_point ecsx, sds_point ecsy, sds_point ecsz);
short		   ic_assoc(struct resbuf *elist, short tp);

// This version doesn't set a global
// Note--this returns the real thing--a pointer into the list, NOT a copy
//
void		   ic_relOdDbResbuf (struct resbuf *prbList);
struct resbuf *ic_ret_assoc( struct resbuf *prbList, int type );
char		  *ic_realloc_char(char * p, size_t n);
void		   ic_add (sds_point pt1, sds_point pt2, sds_point pt3);
double		   ic_atan2(double yy, double xx);
void		   ic_arcextent(sds_point cc, sds_real rr, sds_real sa, sds_real ea, sds_point ll, sds_point ur);
short		   ic_arcxarc(sds_point c0, sds_real r0, sds_real start0, sds_real end0, sds_point c1, sds_real r1, sds_real start1, sds_real end1, sds_point i0, sds_point i1);
void		   ic_authcrc(char *mm, unsigned short nc, char *crc, short incmd);
short		   ic_bin2hex(char *bin, char *hex, short len);
long		   ic_brange(const void *key, const void *base, size_t nelem, size_t width, int (*fcmp)(const void *, const void *), long *preidx, long *postidx);
short		   ic_bulge2arc(sds_point p0, sds_point p1, sds_real bulge, sds_point cc, sds_real *rr, sds_real *sa, sds_real *ea);
void		   ic_catdwgname(char *n);
char		  *ic_centtext(char *ss1, short maxlen);
int 		   ic_charbuff(char **buff, int maxkb, int marginkb);
short		   ic_charok(short cc, char fldtype);
short		   ic_chkext(char *pathname, char *extension);
short		   ic_chkmem(void);
short		   ic_chktimedate(char *ss, short std);
void		   ic_cleanfile(char *string1);
void		   ic_closemess(void);
void		   ic_closeprogress(void);
short		   ic_cmdend(void);
short		   ic_cmdst(void);
short		   ic_colornum(LPCTSTR colstr);
char		  *ic_colorstr(short colnum, char *resstr);
short		   ic_compranges(char *op, long pre, long post, long nelem, long *b0, long *e0, long *b1, long *e1);
short		   ic_connss(sds_name ename, sds_real dist, struct resbuf *filt, sds_name rss);
void		   ic_convertctrlstr(char *ss);
void		   ic_crossproduct(double *a, double *b, double *c);
short		   ic_curlayer(char *clay);
void		   ic_decrypt(char *ss);
void		   ic_delay(unsigned short ms);
short		   ic_delxdata(sds_name, char *, char *);
int			   ic_direction_equal(sds_point p1,sds_point p2, sds_point p3, sds_point p4);
sds_real	   ic_dist2d (sds_point p1,sds_point p2);
short		   ic_divmeas(sds_name ename, sds_real dmval, char *bname, sds_real bscl, sds_real brot, short maxnpts, short mode, struct resbuf **ptlistp);
void		   ic_dotstr(char *ss, short len, LPCTSTR be);
double			ic_dotproduct(double *a,double *b);
short		   ic_dragobject(short mode, short dragit, const sds_point pt1, const sds_point pt2, const sds_real ang, const char *prompt, struct resbuf **retrb, sds_point retpt, char *kword);
void		   ic_drawarc(short color,short hl,short mode,sds_real viewht,sds_real r0,sds_real r1,sds_real r2,sds_real r3,sds_real r4,sds_real elev);
short		   ic_eed2att(sds_name pename, char *appname);
short		   ic_ehilite(sds_name ename, sds_real beg, sds_real end, short fractmode, short color, sds_real width);
short		   ic_ellpts(sds_point cent, sds_point zeropt, sds_real dist2,sds_real sang, sds_real eang, short angmode, short full, short reso,short *nptsp, sds_point **parrayp);
void		   ic_encrypt(char *ss);
char		  *ic_endquote(const char *str, short nchars);
short		   ic_entxss(sds_point p1, sds_point p2, sds_name ss, sds_real fuzz,
						 struct resbuf **ptlistp, long *nfound,int mode3d);
short		   ic_escconv(char *sour, char *dest, char escflag, LPCTSTR conv, short special, short mode);
void		   ic_escstr(char *ss, short which);
void		   ic_exit(short xitcd);
short		   ic_fastview(char *progname, char *argpn, char *imagebox, sds_hdlg arghdlg);
short		   ic_filecopy(char *title, char *filein, char *fileout, short ask, short newdisk);
short		   ic_filelocked(char *ss, char *retmsg);
short		   ic_findblk(char *ts);
int 		   ic_findfile(const char *fname,char *result);
short		   ic_findfirst(char *path, char *match, short mode, char *firstfile);
int			   ic_findinpath( LPCTSTR specificPath, const char *szLookFor, char *szPathFound, DWORD bufSize = IC_ACADBUF);
const char *   ic_getvisiosharedpath( void );
short		   ic_findnext(char *nextfile);
void		   ic_fixpath(char *ss);
short		   ic_fixpwd(char *progfn, char *sn, short mode, short auto1);
long		   ic_freedisk(char *freepath);
long		   ic_gcd(long n1, long n2, unsigned char *pfbuf, long pfbufsz);
short		   ic_genfile(char *prog, char *name, char *type);
short		   ic_getentry(char *pmpt, char *ret, short fldlen, char fldtype);
void		   ic_getext(char *pathname, char *result);
int 		   ic_getmess(char *mess, short sleepsecs, char *ret, short fldlen);
short		   ic_gettimedate(char *tm, char *dt);
short		   ic_getuserid(char *workpath, char *uid);
short		   ic_getxdata(sds_name ename, char *ivar, char *ival, char *appname);
long		   ic_greg2jul(int inputYear,int inputMonth,int inputDay);
short		   ic_handmath(char *hand1, char *hand2, char *result, short op);
char *			ic_handleStringToUpperCase( char *string );
short		   ic_helpmess(char *helpfile, char *helpkey);
short		   ic_hex2bin(char *hex, char *bin, short maxlen);
void			ic_identity(double (*a)[3]);
short		   ic_inorout(struct ic_pointsll *ptsll, sds_real extl, sds_real extb, sds_real extr, sds_real extt, sds_real x0, sds_real y0);
void		   ic_invertmatrix(sds_matrix mtx, bool & invertible, sds_matrix inv);
int				ic_isnumstr(char *ss, double *retval);
bool			ic_strIsValidReal(char *strVal, const bool allowNegative, const bool chewWhiteSpace, sds_real *realVal);
short		   ic_isvalidcol(char *col);
short		   ic_isvalidlt(char *lt, short cont);
short		   ic_isvalidname(char *ss);
sds_real	   ic_isstyleok(void);
short		   ic_linexarc(sds_point p0, sds_point p1, sds_point cc, sds_real rr, sds_real start, sds_real end, sds_point i0, sds_point i1);
short		   ic_linexline(const sds_point p0,const sds_point p1,const sds_point p2,const sds_point p3,sds_point xpt);
int				ic_linexplane(const sds_point start, const sds_point end, const sds_point root, const sds_point normal, sds_point x);
void		   ic_lispclean(void);
void		   ic_listdxf(struct resbuf *elist);
sds_real	   ic_llarea(struct ic_pointsll *ptsll);
short		   ic_loadstart(char *pn, short startup, short (* fnvarval)(char *varp, struct resbuf *rbp));
short		   ic_lockfile(char *workpath, char *ss);
void		   ic_locklook(short use_win);
short		   ic_lockname(char *pn);
void		   ic_lpad(char *ss, short len);
short		   ic_make2darray(char ***ptr, short lns, short chrs);
void		   ic_makebak(char *pn);
short		   ic_makelayer(char *mn, char *ly, char *cl, char *lt);
void		   ic_midpoint(sds_point a, sds_point b, sds_point mid);
void		   ic_mirrorLine(sds_point pt1OnLine, sds_point pt2OnLine, bool & ok, sds_matrix mtx);
void		   ic_mirrorPlane(sds_point ptOnPlane, sds_point vecNormalToPlane, bool & ok, sds_matrix mtx);
long		   ic_mkprimes(long through, unsigned char **pfbufpp);
struct resbuf *ic_newlink(short dxfcd, int typ, void *val);
short		   ic_noresale(void);
void		   ic_normang(sds_real *a1, sds_real *a2);
void			ic_condenseAngle(sds_real& value);
void			ic_nearest(const sds_point s, const sds_point e, const sds_point p, sds_point n);
char		  *ic_nfgets(char *s, short n, FILE *stream);
void		   ic_openprogress(char *title, char *msg, short varsize, void *beg, void *end, short mode, short use_win, short random);
short		   ic_ortho(sds_point base, sds_real snapang, sds_point before, sds_point after, int sstyle);
short		   ic_packxdata(sds_name ename, char **ss, char *appname);
void		   ic_polar(const sds_point ptPolarCtr, double dAngle, double dDistance, sds_point ptPoint);
short		   ic_primefac(long num, unsigned char *pfbuf, long pfbufsz, long *pfa);
void		   ic_progress(void *currval);
short		   ic_ptlndist(sds_point pp, sds_point p0, sds_point p1, sds_real *dist, sds_point ii);
short		   ic_ptsegdist(sds_point pp, sds_point p0, sds_point p1, sds_real bulge, sds_real *dist);
short		   ic_puttimerec(char *workpath, char *cd, char *pg, char *us, char *t1, char *d1, char *t2, char *d2, char *what, char *cs);
short		   ic_putxdata(sds_name ename, char *ivar, char *ival, char *appname);
void		   ic_qsort(char *base,unsigned int nel,unsigned int width,int (*comp_fp)(const void *, const void *));
void		   ic_randfile(char *string1);
short		   ic_registerapp(short tellem, char *appname);
short		   ic_regser(void);
short		   ic_resval(short type);
void		   ic_rotaxis(sds_point axis, double angle, sds_point newxdir, sds_point newydir);
void		   ic_rotxform(sds_point basept, double theta, sds_matrix mtx);
void		   ic_rotxform(sds_point basept, sds_point extrusionvec, double theta, sds_matrix mtx);
void		   ic_rotxform(sds_point uFrom, sds_point uTo, sds_matrix mtx);
void		   ic_rpad(char *ss, short len);
short		   ic_segdist(sds_point pp, sds_point p0, sds_point p1, short type, sds_real *dist, sds_point nearpt);
short		   ic_segxseg(sds_point s0p0, sds_point s0p1, short type0, sds_point s1p0, sds_point s1p1, short type1, sds_real fuzz, sds_point i0, sds_point i1);
int 		   ic_set_by_names(void);
void		   ic_setcrypt(void);
void		   ic_setext(char *pn, char *ext);
short		   ic_setlayer(char *lay);
int			   ic_setpath( char *filename, char *pathname );
void		   ic_settimeeditfile(char *workpath);
short		   ic_setunits(char *ss);
short		   ic_setupum(char um);
void		   ic_setxform(sds_real ang, sds_real xs, sds_real ys, sds_real xi, sds_real yi, sds_real *xfa);
short		   ic_setxmatrix(sds_real *q0, sds_real *q1, sds_real *q2, sds_real *q3, sds_matrix xm, short mode);
short		   ic_ss2dexts(sds_name ss, sds_point ll, sds_point ur);
long		   ic_sscpy(sds_name ss2, sds_name ss1);
/*DG - 31.10.2001*/// Added interOption parameter. Currently it used in COM and in snapping to intersections (sdsengine_osnap function).
// Also added bExtendAnyway which is used in snapping.
short		   ic_ssxss(sds_name ss0, sds_name ss1, sds_real fuzz, struct resbuf **ptlistp, int mode3d, long *nskipped0p, long *nskipped1p, EInterExtendOption interOption = eExtendNone, bool bExtendAnyway = true);
void		   ic_sleep(unsigned short sec);
struct resbuf *ic_sortlist(struct resbuf *rbb);
short		   ic_stoptime(char *workpath, char *cs);
char		  *ic_strcat(char *s1, const char *s2);
char		  *ic_strchr(const char *s1, int c1);
int 		   ic_strcmp(const char *s1, const char *s2);
char		  *ic_strcpy(char *s1, const char *s2);
size_t		   ic_strcspn(const char *p1, const char *p2);
int 		   ic_stricmp(const char *s1, const char *s2);
LPCTSTR		   ic_stristr(LPCTSTR ss1, LPCTSTR ss2);
inline
LPTSTR		   ic_stristr(LPTSTR ss1,  LPCTSTR ss2)
				{
				return const_cast<LPTSTR>(ic_stristr((LPCTSTR)ss1,ss2));
				}
size_t		   ic_strlen(const char *ss);
char		  *ic_strlwr(char *ss);
char		  *ic_strncat(char *s1, char *s2, size_t nn);
int 		   ic_strncmp(const char *s1, const char *s2, size_t nn);
void		   ic_strncpy( LPTSTR d, LPCTSTR s, size_t i);
int 		   ic_strnicmp(const char *s1, const char *s2, size_t nn);
char		  *ic_strnset(char *s1, int c1, size_t nn);
char		  *ic_strpbrk(const char *s1, const char *s2);
char		  *ic_strrchr(const char *s1, int c);
char		  *ic_strrev(char *ss);
char		  *ic_strset(char *ss, int c1);
size_t		   ic_strspn(const char *p1, const char *p2);
char		  *ic_strstr(char *ss1, char *ss2);
double		   ic_strtod(const char *p,char **endp);
float		   ic_strtof(const char *p,char **endp);
char		  *ic_strtok(char *p1, const char *p2);
char		  *ic_strupr(char *ss);
void		   ic_sub (sds_point pt1, sds_point pt2, sds_point pt3);
void		   ic_swap (sds_point & p, sds_point & q);
short		   ic_titlefill(char *pname, char **sa);
void		   ic_trim(char *ss, char *be);
void		   ic_unitize(sds_point p);
void		   ic_unitvec (sds_point pt1, sds_point pt2, sds_point pt3);
short		   ic_unlockfile(char *ss);
int			   ic_usemat(sds_point sour, sds_point dest, void *mat, int disp, int oldmattype, int inverse);
short		   ic_vdirins(sds_point vd, sds_point extru, sds_real *rotp);
double		   ic_veclength(sds_point p);
short		   ic_viewfile(char *progname, char *filename);
short		   ic_xdfromll(struct resbuf *lelist, char *ivar, char *ival, char *appname);
short		   ic_xdtoll(struct resbuf *lelist, char *ivar, char *ival, char *apname);
void		   ic_xform(sds_real x0, sds_real y0, sds_real *x1, sds_real *y1, sds_real *xfa, short dir);
void		   ic_xformpt(sds_point from, sds_point to, sds_matrix mat);
int			   ic_isvalid_real( sds_real rInput );
void			ic_roundReal(sds_real& value);
void			ChangeSlashToBackslashAndRemovePipe(char *str);
void			ic_idmat (sds_matrix m);
void			ic_transmat (double x, double y, double z, sds_matrix m);
void			FreeResbufIfNotNull(resbuf **rb);
void			ic_transpose(sds_matrix);
void			ic_copy(sds_matrix copy, sds_matrix original);

/*-------------------------------------------------------------------------*//**
Compares the given double with zero and sets it to exact zero if it's near to it.

@author	Dmitry Gavrilov
@param d <=> a double to check and fix
*//*--------------------------------------------------------------------------*/
inline
void ic_fixZeroReal(double& d)
{
	if(fabs(d) < IC_ZRO)
		d = 0.0;
}


/*-------------------------------------------------------------------------*//**
Compares the given 2 doubles with zero and sets them to exact zero if they're near to it.

@author	Dmitry Gavrilov
@param pD <=> a pointer to 2 doubles to check and fix, must not be NULL
*//*--------------------------------------------------------------------------*/
inline
void ic_fixZeroReal2(double* pD)
{
	if(fabs(pD[0]) < IC_ZRO)
		pD[0] = 0.0;
	if(fabs(pD[1]) < IC_ZRO)
		pD[1] = 0.0;
}


/*-------------------------------------------------------------------------*//**
Compares the given 3 doubles with zero and sets them to exact zero if they're near to it.

@author	Dmitry Gavrilov
@param pD <=> a pointer to 3 doubles to check and fix, must not be NULL
*//*--------------------------------------------------------------------------*/
inline
void ic_fixZeroReal3(double* pD)
{
	if(fabs(pD[0]) < IC_ZRO)
		pD[0] = 0.0;
	if(fabs(pD[1]) < IC_ZRO)
		pD[1] = 0.0;
	if(fabs(pD[2]) < IC_ZRO)
		pD[2] = 0.0;
}



#if	defined(DBDLL) || defined(DBSTATIC)
#include "db.h"		// must be defined for DBDLL files
#endif

#ifndef	DB_CLASS
	#define	DB_CLASS	__declspec( dllimport)
#endif

	// ResourceString -- class for loading resource strings
	// WARNING - NOT MByte Enabled - use a BSTR..
	// and don't create static string in production (resource only)
#ifdef	NDEBUG
#define	_ResourceString(id,string)	(__ResourceString(id,NULL))
#else
#define	_ResourceString(id,string)	(__ResourceString(id,string))
#endif

#define	ResourceString(id,string)	((LPCTSTR)_ResourceString(id,_T(string)))


class DB_CLASS __ResourceString
	{
public:
	__ResourceString( int resourceID, LPCTSTR t);
	~__ResourceString();
	operator LPCTSTR();

protected:
	LPCTSTR			m_loaded;
	LPCTSTR			m_string;
	int				m_id;
	TCHAR			m_buffer[32];
	};


#if defined(__cplusplus)
//	}
#endif



#ifndef _SINGLETHREADSECTION_H
	#include "singlethreadsection.h"
#endif

#ifndef _LOCKABLEOBJECT_H
	#include "lockableobject.h"
#endif

#include "..\..\icad\IcadUtil.h"

#endif // _ICADLIB_H

