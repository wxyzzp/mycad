/* D:\ICAD\PRJ.MAIN\LIB\DB\DB_UTIL.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"/*DNT*/
#include "appid.h"
#include "dimstyletabrec.h"
#include "ltypetabrec.h"
#include "styletabrec.h"
#include "ucstabrec.h"
#include "viewtabrec.h"
#include "vporttabrec.h"
#include "vxtabrec.h"
#include "objects.h"
#include "DbAcisEntity.h"
#include "DbGroup.h"
#include "DbPlotSettings.h"
#include "DbLayout.h"
#include "DbMText.h"
#include "DbSortentsTable.h"
#include "greedyheap.h"

/********************************************************************/
/************************* MEMBER FUNCTIONS *************************/
/********************************************************************/

/***** db_charbuflink *****/

/*F*/
DB_API db_charbuflink::db_charbuflink(void) 
{
	buf=bufcopy=NULL; 
	size=seekof0=0; 
	next=NULL;
}

DB_API db_charbuflink::db_charbuflink(int sz, int clearit) 
{
	buf=bufcopy=NULL; 
	size=seekof0=0; 
	next=NULL;
	if (sz<1) 
		return;
	size=sz; 
	buf=new char[size];

	if (clearit) 
		memset(buf,0,size);
}
DB_API db_charbuflink::~db_charbuflink(void) 
{
	if(buf!=NULL) 
		delete [] buf;
	if (bufcopy!=NULL) 
		delete [] bufcopy;
	buf=NULL; 
	bufcopy=NULL;
}

short charToHexval(char c)
{
  if (c >= '0' && c <= '9') 
	  return(short)(c - '0');
  if (c >= 'A' && c <= 'F') 
	  return(short)(c - 'A'+10);
  if (c >= 'a' && c <= 'f') 
	  return(short)(c - 'a'+10);
  return(0);
}


DB_API void db_AsciiHexToBinary (char *readBuffer,char *storeBuffer, int storeBufferLen, short *validChars)
{
	*validChars=0;
	short i,value;

	short charsRead = 0;
	short length=(short)strlen(readBuffer);

	if (length & 1)   // Process the first character separately
	{ 
		*storeBuffer++=(char)charToHexval(*readBuffer++);
		length--;
		(*validChars)++;
		charsRead++;
	}

	short charsLeft = (short)(length / 2);
	(*validChars) += charsLeft;
	if ((*validChars) > storeBufferLen)
		(*validChars) = storeBufferLen;

	for (i=0; i<charsLeft && i<storeBufferLen; i++) 
	{
		value=(short)(charToHexval(*readBuffer++)*16);
		value+=charToHexval(*readBuffer++);
		*storeBuffer++ = (char)(value & 0xFF);
		charsRead+=2;
	}

	return;
}

DB_API void db_BinaryHexToAscii (unsigned char *binData,short binDataLength,char *tempStr, int iTempStrLen)
	{
	while (binDataLength--) 
	{
		sprintf(tempStr, "%.2X", (((unsigned)*binData++) & 255));

		tempStr+=2;
		iTempStrLen-=2;
		if (2 > iTempStrLen)
			return;
	}
}


/*F*/
DB_API int db_aextrucpy(sds_real **extrup, sds_point newextru) {
/*
**	Copies newextru to *extrup and unitizes it.
**	If newextru is NULL or (0,0,1), *extrup is deleted (if necessary)
**	and set to NULL.
**
**	This is for ALLOCATED extrusions only (where NULL signifies (0,0,1)).
**
**	Returns:
**		 0 : OK
**		-1 : extrup==NULL
*/
	sds_point ap1;
	ap1[0] = ap1[1] = ap1[2] = 0.0; // EBATECH(CNBR) Need Initialize

	if (extrup==NULL) return -1;

	if (newextru==NULL) ap1[2]=1.0;
	else { ic_ptcpy(ap1,newextru); if (db_unitizevector(ap1)) ap1[2]=1.0; }
	if (fabs(ap1[2]-1.0)<1.0E-10) {
		delete [] *extrup; *extrup=NULL;
	} else {
		if (*extrup==NULL) *extrup=new sds_real[3];
		ic_ptcpy(*extrup,ap1);
	}

	return 0;
}


/*F*/
DB_API void db_astrncpy(char** cpp, const char* newval, int maxnc) {
/*
**	NOTE: Uses new/delete.	Don't use for strings that will be free()d.
**
**	This is a strncpy for an ALLOCATED string, freeing the old
**	value first.
**
**	Given the ADDRESS of a char POINTER to an ALLOCATED string (cpp),
**	this function deletes the old value (*cpp) and allocates it again for
**	up to maxnc chars of newval.
**
**	If newval is NULL, this function just frees the old value.
**
**	If maxnc<0, strlen(newval) will be used.
**
*/
	int howmany,newlen;

	if (cpp==NULL)
		return;
	delete [] *cpp;
	*cpp=NULL;
	if (newval==NULL)
		return;
	newlen=strlen(newval);
	if (maxnc<0)
		howmany=newlen;
	else
		howmany=min(newlen,maxnc);
	*cpp=new char[howmany+1];
	strncpy(*cpp,newval,howmany);
}


/*F*/
DB_API void db_mstrncpy(char** cpp, const char* newval, int maxnc)
	{
/*
**	NOTE: Uses malloc/free.  Don't use for strings that will be delete()d.
**
**	This is a strncpy for an ALLOCATED string, freeing the old value first.
**
**	Given the ADDRESS of a char POINTER to an ALLOCATED string (cpp),
**	this function deletes the old value (*cpp) and allocates it again for
**	up to maxnc chars of newval.
**
**	If newval is NULL, this function just frees the old value.
**
**	If maxnc<0, strlen(newval) will be used.
**
*/
	int howmany,newlen;

	if (cpp==NULL)
		return;
	if (*cpp!=NULL)
		delete [] (*cpp);
	*cpp=NULL;
	if (newval==NULL)
		return;
	newlen=strlen(newval);
	if (maxnc<0)
		howmany=newlen;
	else
		howmany=min(newlen,maxnc);
	*cpp= new char [howmany+1];
	strncpy(*cpp,newval,howmany);
	}

/*F*/
DB_API int db_chkrange(const resbuf *rbp, char *range) {
/*
**	Checks the value in the resbuf pointed to by rbp against the
**	range-specifier string "range".
**
**	If "range" is NULL or blank, there are no range restraints.
**
**	Currently, only numeric ranges are acceptable, and there must
**	an underbar:
**
**		A_B :	 A <= val <= B
**
**		 A_ :	 A <= val
**
**		 _B :		  val <= B
**
**	Types other than RTSHORT, RTLONG, and RTREAL are always in-range.
**
**	Returns:
**		 0 : Value in range
**		-1 : Value out of range
*/
	char *minp,*maxp;

	minp=maxp=NULL;

	if (rbp==NULL) return -1;
	if (range==NULL || (rbp->restype!=RTSHORT && rbp->restype!=RTLONG &&
		rbp->restype!=RTREAL)) return  0;

	minp=range; while (isspace((unsigned char) *minp)) minp++;
	if (!*minp) return 0;  /* Range string is blank. */
	maxp=minp;
	if (*minp=='_'/*DNT*/) {
		minp=NULL;
	} else {
		while (*maxp && *maxp!='_'/*DNT*/) maxp++;
		if (!*maxp) return 0;  /* No underbar found. */
	}
	maxp++;  /* Step over underbar. */
	while (isspace((unsigned char) *maxp)) maxp++;
	if (!*maxp) maxp=NULL;

	if (minp==NULL && maxp==NULL) return 0;

	switch (rbp->restype) {
		case RTSHORT:
			if ((minp!=NULL && rbp->resval.rint<atoi(minp)) ||
				(maxp!=NULL && rbp->resval.rint>atoi(maxp))) return -1;
			break;
		case RTLONG:
			if ((minp!=NULL && rbp->resval.rlong<atol(minp)) ||
				(maxp!=NULL && rbp->resval.rlong>atol(maxp))) return -1;
			break;
		case RTREAL:
			if ((minp!=NULL && rbp->resval.rreal<atof(minp)) ||
				(maxp!=NULL && rbp->resval.rreal>atof(maxp))) return -1;
			break;
	}

	return 0;
}



/*F*/
DB_API int db_comphand(const char *hand1, const char *hand2) {
/*
**	Compares two handles in hex string form.
**
**	Returns:
**		-1 : hand1< hand2
**		 0 : hand1==hand2
**		+1 : hand1> hand2
*/
	int fi1,fi2;

	static char wh[2][DB_MAXHANDSTRLEN+1];


	if		(hand1==NULL) return (hand2==NULL) ? 0 : -1;
	else if (hand2==NULL) return 1;

	wh[0][DB_MAXHANDSTRLEN]=wh[1][DB_MAXHANDSTRLEN]=0;

	/* Lpad both to DB_MAXHANDSTRLEN chars: */
	for (fi1=0; fi1<DB_MAXHANDSTRLEN && hand1[fi1]; fi1++);
	fi1--;
	for (fi2=DB_MAXHANDSTRLEN-1; fi2>-1; fi2--,fi1--)
		wh[0][fi2]=(fi1>-1) ? hand1[fi1] : ' ';
	for (fi1=0; fi1<DB_MAXHANDSTRLEN && hand2[fi1]; fi1++);
	fi1--;
	for (fi2=DB_MAXHANDSTRLEN-1; fi2>-1; fi2--,fi1--)
		wh[1][fi2]=(fi1>-1) ? hand2[fi1] : ' ';

	return db_compareHandleStrings(wh[0],wh[1]);
}



/*F*/
DB_API int db_convhand(char *sour, char *dest, int sform, int dform) {
/*
**	Converts handle sour from one form to another in dest.
**	(Both may have imbedded 0's for the non-string forms.)
**
**	sform and dform are the source and destination forms:
**
**		0 : 8-byte form (as in EED).  Always 8 bytes with leading 0's.
**			  Each char hold 2 hex digits.
**			  Example: 0,0,0,0,0,0,0x03,0xA5
**
**		1 : Length/value form (as in entities).  A char indicating
**			  how many bytes follow, followed by the bytes (each
**			  holding 2 hex digits, as in the 8-byte form above.
**			  Example: 0x02,0x03,0xA5
**
**		2 : String (as in LISTs, DXF, and entget()'s.  A 0-terminated
**			  string of hex digits.  Example: "3A5"
**
**	The caller must make sure dest can hold the number of chars this
**	function will generate (DB_MAXHANDSTRLEN+1 will cover everything).
**
**	sour and dest can refer to the same string (since sour is used
**	to make intermediate inter before dest is used).
**
**	Returns:
**		 0 : OK
**		-1 : One or both passed ptrs is NULL, or bad form code(s)
**		-2 : sour invalid for its form
*/
	unsigned char inter[8];
	char fs1[81];
	int fi1,fi2;


	if (sour==NULL || dest==NULL || sform<0 || sform>2 ||
		dform<0 || dform>2) return -1;

	/* Convert to 8-byte form as an intermediate: */
	if (sform==0) {
		memcpy(inter,sour,8);
	} else if (sform==1) {
		if ((fi1=(int)(*sour))<0 || fi1>8) return -2;
		fi1=8-fi1;	/* # of leading 0's */
		for (fi2=0; fi2<8; fi2++)
			inter[fi2]=(fi2<fi1) ? 0 : sour[1+fi2-fi1];
	} else {
		if ((fi1=strlen(sour))>16) return -2;

		/* Make an upper case copy padded to 16 chars with leading '0's: */
		memset(fs1,'0',16); strcpy(fs1+16-fi1,sour);

		ic_handleStringToUpperCase(fs1);

		/* Convert each pair to binary form in inter: */
		for (fi1=0; fi1<8; fi1++) {
			fi2=2*fi1;

// Following lines of code are commented out because they are SLOW!!!!
// Was taking most of the time in this function
// Ultimately, don't use strings.  In the meantime, code wasn't checking this
// return value anyhow, so no harm done (hopefully).
//
//			  if (!isxdigit((unsigned char) fs1[fi2]) || !isxdigit((unsigned char) fs1[fi2+1]))
//				{
//				return -2;
//				}


			inter[fi1]=
				((fs1[fi2]-((fs1[fi2]>='0'/*DNT*/ && fs1[fi2]<='9'/*DNT*/) ?
					'0'/*DNT*/ : 'A'/*DNT*/-10))<<4) |
				(fs1[fi2+1]-((fs1[fi2+1]>='0'/*DNT*/ && fs1[fi2+1]<='9'/*DNT*/) ?
					'0'/*DNT*/ : 'A'/*DNT*/-10));
		}
	}

	/* Convert from 8-byte form to dform: */
	if (dform==0) {
		memcpy(dest,inter,8);
	} else if (dform==1) {
		fi1=0; while (fi1<8 && !inter[fi1]) fi1++;
		/* fi1 is now the # of leading 0's in inter (and the idx */
		/* of the first non-0 char). */
		dest[0]=8-fi1;
		while (fi1<8) { dest[1+fi1-(8-dest[0])]=inter[fi1]; fi1++; }
	} else {
		/* Put the string into fs1 so that a leading '0' will be */
		/* easy to skip. */

		fi1=0; while (fi1<8 && !inter[fi1]) fi1++;
		/* fi1 is now the idx of the first non-0 char. */

		register char *cptr=fs1;
		char hexval[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
		for (; fi1<8; fi1++) {
			*cptr++=hexval[(inter[fi1])>>4];
			*cptr++=hexval[inter[fi1]&'\x0F'/*DNT*/];
		}
		*cptr=0;

		strcpy(dest,fs1+(*fs1=='0'/*DNT*/));
	}

	return 0;
}




/*F*/
DB_API int db_str2rb(char *str, resbuf *rbp) {
/*
**	Given a string str representing a value and a
**	resbuf pointer rbp that points to an existing resbuf,
**	this function uses str to set rbp->resval based on the type
**	specified by rbp->restype.
**
**	For a point (RTPOINT or RT3DPOINT), str must have commas
**	between the coordinates: "XXX[,YYY[,ZZZ]]".  Unspecified
**	Y and Z values are set to 0.0.
**
**	CALLER RESPONSIBILITIES:
**
**	  Note that the caller must free any previous string value
**	  and set rbp->restype to the desired type before calling
**	  this function.
**
**	  The caller must also free a string value set by this function
**	  (rbp->restype==RTSTR && rbp->resval.rstring!=NULL).
**
**	Returns:
**		 0 : OK
**		-1 : Calling error (str==NULL || rbp==NULL)
**		-2 : No memory for a string value.	(No longer possible.)
**		-3 : Improper type or a type not handled yet
*/
	char *fcp1;

	if (str==NULL || rbp==NULL) return -1;

	switch (rbp->restype) {
		case RTSHORT:
			rbp->resval.rint=atoi(str);
			break;
		case RTLONG:
			rbp->resval.rlong=atol(str);
			break;
		case RTREAL: case RTANG: case RTORINT:
			rbp->resval.rreal=atof(str);
			break;
		case RTSTR:
			db_astrncpy(&rbp->resval.rstring,str,SHRT_MAX);
			break;
		case RTPOINT: case RT3DPOINT:
			fcp1=str;
			rbp->resval.rpoint[0]=atof(fcp1);
			while (*fcp1 && *fcp1!=',') fcp1++;
			rbp->resval.rpoint[1]=(*fcp1) ? atof(++fcp1) : 0.0;
			if (rbp->restype==RTPOINT) break;
			while (*fcp1 && *fcp1!=',') fcp1++;
			rbp->resval.rpoint[2]=(*fcp1) ? atof(++fcp1) : 0.0;
			break;
		default:
			return -3;
	}

	return 0;
}


/*F*/
DB_API int db_unitizevector(sds_point vect) {
/*
**	Unitizes vect, if possible.
**
**	Returns:
**		 0 : OK
**		-1 : vect is NULL or the zero-vector.
*/
	int rc=-1;
	sds_real ar1;

	if (vect!=NULL &&
		!icadRealEqual((ar1=sqrt(vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2])),0.0)) {

		if (!icadRealEqual(ar1,1.0)) { vect[0]/=ar1; vect[1]/=ar1; vect[2]/=ar1; }
		rc=0;
	}
	return rc;
}


// ***************************************
// ALLOCATE AND FREE RESBUF FUNCTIONS
//
//
// The first set uses SMARTHEAP and a fixed size pool to try and optimize
// the allocations.
// The second version uses CRT routines (which may call through to SmartHeap)
//
#if 0
#include <smrtheap.h>

static MEM_POOL	gResBufMemoryPool = NULL;

static void initResBufMemoryPool(void)
	{
	gResBufMemoryPool = MemPoolInitFS( sizeof( struct resbuf ), 100, 0 );
	ASSERT( gResBufMemoryPool != NULL );
	}

struct resbuf *
db_alloc_resbuf( void )
	{
	if ( gResBufMemoryPool == NULL )
		{
		initResBufMemoryPool();
		}
	struct resbuf *retval = (struct resbuf *)MemAllocFS( gResBufMemoryPool );
	if ( retval != NULL )
		{
		memset( retval, 0, sizeof( *retval ) );
		}
	return retval;
	}


void
db_free_resbuf( struct resbuf *rb )
	{
		ASSERT( rb != NULL );
		MemFreeFS( rb );
	}

#else	 // USE_SMARTHEAP

// *********************************
// These versions do not depend on SmartHeap
//

#define	COUNTOF(array)	(sizeof(array)/sizeof((array)[0]))

	// simple spin lock
class SpinLock
	{
public:
	SpinLock()
		{
		while ( InterlockedDecrement( &m_lockGuard) )
			InterlockedIncrement( &m_lockGuard);
		}
	~SpinLock()
		{
		InterlockedIncrement( &m_lockGuard);
		}
private:
	static LONG	m_lockGuard;
	};

LONG	SpinLock::m_lockGuard = 1;

static struct resbuf *	resbufCache[200] = {NULL};
static LONG	resbufCacheAvailable = 0;

	// clean up cache in static destructor
class CacheCleanup
	{
public:
	~CacheCleanup()
		{
		for ( int i=0; i < resbufCacheAvailable; i++ )
			delete  resbufCache[i];
		}
	};

static CacheCleanup	cleanup;

//<alm: old implementation of free/alloc resbuf>
#if 0
void
db_free_resbuf( struct resbuf *rb )
	{
	SpinLock	lock;

	ASSERT( rb != NULL );

	if ( resbufCacheAvailable < COUNTOF( resbufCache) )
		resbufCache[ resbufCacheAvailable++] = rb;
	else
		delete  rb ;
	}

struct resbuf *
db_alloc_resbuf( void )
	{
	SpinLock	lock;

	struct resbuf *retval = NULL;

		// get cached resbuf if available
	if ( resbufCacheAvailable )
		{
		retval = resbufCache[ --resbufCacheAvailable];
		retval->restype = 0;
		retval->resval.rstring = NULL;
		retval->rbnext = NULL;
		// cleaning up reused pointers
		retval->resval.rbinary.clen = 0;
		retval->resval.rbinary.buf = NULL;
		}

	if ( !retval )
	{
		retval = new struct resbuf ;
		memset(retval,0,sizeof( struct resbuf));
	}

	return retval;

	}

#else
//<alm: new implementation uses greedy heap,
//		which allocates memory by large pieces, and free it only when it's destroyed>
static icl::greedy_heap<resbuf> s_resbufHeap(offsetof(resbuf, rbnext));

/*-------------------------------------------------------------------------*//**
Alternative (to SpinLock) implementation of synchronization mechanizm
with usage of CRITICAL_SECTION
@author Alexey Malov
*//*--------------------------------------------------------------------------*/
class lock
{
	CRITICAL_SECTION _section;

public:
	struct sentry
	{
		lock& _l;
		sentry(lock& l)
			:
		_l(l)
		{
			EnterCriticalSection(&_l._section);
		}
		~sentry()
		{
			LeaveCriticalSection(&_l._section);
		}
	};
	friend struct sentry;

public:
	lock()
	{
		InitializeCriticalSection(&_section);
	}
	~lock()
	{
		DeleteCriticalSection(&_section);
	}
};
//static lock s_lock;

resbuf* db_alloc_resbuf()
{
	resbuf* rb = s_resbufHeap.malloc();
	memset(rb, 0, sizeof(resbuf));

	return rb;
}

void db_free_resbuf(resbuf* rb)
{
	assert(rb);
	s_resbufHeap.free(rb);
}

resbuf* db_alloc_resbuf_list(size_t n)
{
	return s_resbufHeap.mallocl(n);
}

void db_free_resbuf_list(resbuf* rb)
{
	s_resbufHeap.freel(rb);
}

#endif

#endif // USE_SMARTHEAP

/*F*/
struct resbuf *db_newrb(short rtype, int valtype, void *valp) {
/*
**
**	BASED ON ic_newlink() -- BUT I'VE REMOVED THE STRING LENGTH
**	RESTRICTION AND ALLOWED IT TO ALLOC "" STRINGS (INSTEAD OF
**	LEAVING THE .RSTRING SET TO NULL).	(I'VE CHANGED OTHER THINGS,
**	TOO.)
**
**	ESPECIALLY NOTE THAT I CHANGED THE POINT LOGIC SO THAT THE
**	CALLER MUST PASS THE POINT AS valp -- NOT THE address OF THE POINT.
**	AN sds_point IS ALREADY A POINTER, SO WHY COMPLICATE THE ISSUE?
**
**	Allocs and fills a struct resbuf.  rtype is the DXF code that
**	will be stored in the restype.	valp is a pointer to whatever the
**	value variable is.	valtype can be the following:
**	  'S' (String)
**	  'I' (Int)    (valp MUST POINT TO AN int VAR -- NOT A short)
**	  'H' (sHort)
**	  'L' (Long)
**	  'R' (Real)
**	  'P' (Point)
**	  'N' (entity Name (sds_name)
**	  'B' (Binary chunk (with 1st byte giving the length))
**			 (NOTE: Not sure sds_relrb frees this!)
**	  Any other valtype will not set a resval, (but the restype will still
**		be set to rtype).  (valp==NULL will have the same effect.)
**
**	WARNING: Don't use db_astrncpy() in here or anything else that
**	uses new/delete; resbufs are still alloc/free items.
**
**	Returns: Addr of the new struct resbuf, or NULL if no memory.
*/
	short fi1;
	struct resbuf *tp1;

//	  valtype=toupper(valtype);
	if ( (valtype >= 'a'/*DNT*/) &&
		 (valtype <= 'z'/*DNT*/) )
		{
		valtype += ('A'/*DNT*/ - 'a'/*DNT*/);
		}

	if (( tp1=db_alloc_resbuf() )==NULL)
		return NULL;

	tp1->restype=rtype;

	if (valtype=='S'/*DNT*/) {
		if (valp==NULL) valp=db_str_quotequote;
		if ((tp1->resval.rstring= new char [strlen((char *)valp)+1])==NULL)
			{ delete [] tp1; return NULL; }
		strcpy(tp1->resval.rstring,(char *)valp);
	} else {

		if (valp==NULL) return tp1;

		if (valtype=='I'/*DNT*/) {
			tp1->resval.rint=*((int *)valp);
		} else if (valtype=='H'/*DNT*/) {
			tp1->resval.rint=*((short *)valp);
		} else if (valtype=='L'/*DNT*/) {
			tp1->resval.rlong=*((long *)valp);
		} else if (valtype=='R'/*DNT*/) {
			tp1->resval.rreal=*((sds_real *)valp);
		} else if (valtype=='P'/*DNT*/) {
			DB_PTCPY(tp1->resval.rpoint,(sds_real *)valp);
		} else if (valtype=='N'/*DNT*/) {
			tp1->resval.rlname[0]=((long *)valp)[0];
			tp1->resval.rlname[1]=((long *)valp)[1];
		} else if (valtype=='B'/*DNT*/) {
			if ((fi1=*((unsigned char *)valp))>127) fi1=127;
			if (fi1) {
				if ((tp1->resval.rbinary.buf= new char [fi1] )==NULL)
					{ delete [] tp1; return NULL; }
				memset(tp1->resval.rbinary.buf,0,fi1);
				memcpy(tp1->resval.rbinary.buf,((char *)valp)+1,fi1);
			}
			tp1->resval.rbinary.clen=fi1;
		}
	}

	return tp1;
}


/*F*/
DB_API void db_setgrafunptrs(void (*grafreedofp)(void *)) {
/*
**	This function is intended to be called by gra_initview() to give
**	db the functions to free certain gra items WITHOUT INCLUDING gra.h.
**	It sets global function pointers in dwgrw.
**
**	(Used to be plural; now it just does gra_freedisplayobjectll().)
*/

	db_grafreedofp=grafreedofp;
}


// These functions return the pointers from the db .DLL
// (Can't put global vars in the def file, apparently.)
/*F*/
DB_API struct db_sysvarlink *db_Getsysvar(void) { return(db_oldsysvar); }
/*F*/

/*F*/
DB_API short db_Getnvars(void) { return(db_nvars); }
/*F*/

/*F*/
DB_API int db_ret_lasterror(void) { return db_lasterror; }


/*F*/
DB_API char *db_hitype2str(int hitype) {
/*
**	Converts an integer hitype to its string equivalent.
**	Examples: given DB_LINE, this function returns a pointer to
**	"LINE".  DB_APPIDTABREC gives "APPID".
**
**	DON'T MODIFY THE STRING.  IT'S A STRING CONSTANT IN GLOBAL
**	MEMORY.  Just use it in strcmps, etc.
*/
	return (db_is_validhitype(hitype)) ?
		db_hitypeinfo[hitype].name : db_str_quotequote;
}


/*F*/
DB_API char *db_sysvarqidx2str(int svidx) {
/*
**	Converts a sysvar DB_Q... value to its string equivalent.
**	Example: Given DB_QCECOLOR, this function returns a pointer to
**	"CECOLOR" (db_sysvar[DB_QCECOLOR].name).
**
**	DON'T MODIFY THE STRING.  IT'S A STRING CONSTANT IN GLOBAL
**	MEMORY.  Just use it in strcmps, etc.
*/
	return (svidx>-1 && svidx<db_nvars) ?
		db_oldsysvar[svidx].name : db_str_quotequote;
}


/*F*/
DB_API int db_str2hitype(char *typestr) {
/*
**	Given a string handitem type, returns the appropriate
**	DB_handitemtype.  Examples: Given "LINE", this function
**	returns DB_LINE.  "APPID" gives DB_APPIDTABREC.
**
**	NOTE SPECIAL CASE.	"BLOCK" is the name of a table type AND
**	an entity (it's the 0-group for both).	This function will return
**	DB_BLOCKTABREC for "BLOCK" (because that's what it finds first
**	in db_hitypeinfo[]).  When that's NOT desired, the caller
**	must make it a special case and not call this function.
**	(See db_drawing::entmake() for an example.)
**
**	Returns -1 if typestr is not valid.
*/
	int rc=-1;

	if (typestr!=NULL) {
		for (rc=0; rc<DB_NHITYPES &&
			db_compareNames(typestr,db_hitypeinfo[rc].name); rc++);
		if (rc>=DB_NHITYPES) rc=-1;
	}

	return rc;
}


/*F*/
int db_tab2tabrecfn(int tabtype) {
/*
**	Example: DB_LAYERTAB returns DB_LAYERTABREC.
**	Returns -1 if the pass parameter is out of range.
*/
	return (tabtype>-1 && tabtype<DB_NTABS) ? db_tab2tabrec[tabtype] : -1;
}


/*F*/
DB_API int db_tabrec2tabfn(int hitype) {
/*
**	Example: DB_LAYERTABREC returns DB_LAYERTAB.
**	Returns -1 if the pass parameter is out of range.
*/
	return (db_is_tabrectype(hitype)) ? db_hitypeinfo[hitype].tabtype : -1;
}


/*F*/
int db_makesvbuf(int type, db_charbuflink *svbufp, db_drawing *dp) {
/*
**	Allocates and/or returns the size of one of the three types of
**	system variable buffers.  Parameter "type" determines which buffer
**	it deals with:
**
**		0 : Header
**		1 : Configuration
**		2 : Session
**
**	If svbufp is non-NULL, it must be a pointer to a
**	db_charbuflink.  The ->buf member will be allocated and set to default
**	values (using setvar with db_sysvar[].defval).	The ->size
**	member will be set to the size of the allocated buffer.
**
**	NOTE: A HEADER buffer is created and filled in the same manner as
**	the CONFIG and SESSION buffers.  That is, ONLY SYSTEM VARIABLES ARE
**	SET.  During the opening of R10 and R12 drawings, the beginning of
**	that buffer will be over-written by reading it directly from the
**	DWG.  When this function is called from elsewhere to make a header
**	buffer however, there are MANY important things still to set to make
**	a valid header -- the initial ACADnnnn ID string, the section
**	pointers, the table size/number/seeker values, the TDCREATE and
**	TDUPDATE settings, the CRC.  The seekers will have to be determined
**	as the file is written.  (It will be the same old scenario we've
**	done before: write the header to save its place, fix the seekers as
**	we write the rest of the file, jump back to the beginning and write
**	the header again.)
**
**	The buffers delt with here are FULL SIZE buffers: a header is
**	created to hold all header variables marked as header vars in
**	db_sysvar[] (and can later be PARTIALLY written out to create R10
**	or R12 headers during the DWG-creation phase).	A CONFIG buffer is
**	created for all vars marked as config vars in db_sysvar[].loc; a
**	SESSION buffer is created for all vars marked as session vars in
**	db_sysvar[].loc (except those flagged as special-source).  (If the
**	CONFIG or SESSION buffers were created different sizes based on
**	release, we'd need a different map for each release; but we have
**	only ONE map, based on db_sysvar[].bc.)
**
**	db_sysvarbufsz[] holds the sizes of the HEADER, CONFIG, and SESSION
**	->buf buffers.	THIS is the function that sets them. The idea is to
**	set them (and map the CONFIG and SESSION buffers) just ONCE
**	(although the caller can cause it to happen again by setting any
**	one of them to 0). If any is <1, this function is triggered to
**	determined all of them AND map the CONFIG and SESSION buffers by
**	setting the db_sysvar[].bc members.  The CONFIG and SESSION vars
**	are assigned slots in their buffers in the order they are
**	encountered in db_sysvar[].  (THE .bc VALS FOR DWG HEADER VARS ARE
**	PRESET AND MUST NOT BE ALTERED; THEY MAP THE HEADERS FOR R10 AND
**	R12.  R13 vars have been assigned slots at the end of the buffer,
**	after the R10 and R12 vars.)
**
**	svbufp can be NULL if you don't want a buffer allocated;
**	the size that the ->buf member would occupy is returned.
**
**	IT'S UP TO THE CALLER TO FREE svbufp->buf IF IT IS ALLOCATED BY THIS
**	FUNCTION.
**
**	Returns:
**		-1 : Calling error.
**		>0 : The size in bytes of the actual char buffer (the .buf member
**			   of the db_charbuflink).	When svbufp is non-NULL,
**			   this size is also stored in svbufp->size.
*/
	int vidx,pass,ntypes,rc,fi1;
	short fshort1;
	resbuf rb;
	db_drawing dummydraw(0);  /* Not initialized */

	rc=0;
	ntypes=sizeof(db_sysvarbufsz)/sizeof(db_sysvarbufsz[0]);
	memset(&rb,0,sizeof(rb));


	/* Delete old buffer (if there is one): */
	if (svbufp!=NULL)
		{ delete [] svbufp->buf; svbufp->buf=NULL; svbufp->size=0; }

	/* If it's time to determine the sizes, init them all to 0: */
	if (db_sysvarbufsz[0]<1 || db_sysvarbufsz[1]<1 ||
		db_sysvarbufsz[2]<1)  /* Count them */
			db_sysvarbufsz[0]=db_sysvarbufsz[1]=db_sysvarbufsz[2]=0;

	if (type<0 || type>=ntypes) { rc=-1; goto out; }

	// VT: Fix. 21-3-02
	for (pass=(db_sysvarbufsz[0]>1 ? 1:0); pass<1+(svbufp ? 1:0); pass++) {
		/* 0 : count, and set .bc vals; 1 : alloc and set defaults. */

		if (pass && db_sysvarbufsz[type]>0) {

			/* Alloc the buffer and set the size: */
			svbufp->newbuf(db_sysvarbufsz[type],1);

			/* Set up dummydraw for setvar (to specify release, header, */
			/* and tables).  DON'T FREE IT LATER: */
			dummydraw.rel=2;  /* Set R13; probably not necessary for this */
								   /* dummy, but... */
			dummydraw.table=(dp!=NULL) ? dp->table : NULL;
			if (type==0) {	/* Header */
				/* Set the header for dummydraw (UNDO THIS BEFORE EXITING!): */
				dummydraw.header.buf=svbufp->buf;
				dummydraw.header.size=svbufp->size;

				/* Set the non-zero table-indexed defaults (so that */
				/* they're okay even if setvar() fails to set them): */

				/* CELTYPE: */
				fshort1=DB_BLLTYPE12;  /* R12 BYLAYER */
				memcpy(svbufp->buf+db_oldsysvar[DB_QCELTYPE].bc,&fshort1,2);
				/* DIMSTYLE: */
				fshort1=-1;  /* "*UNNAMED" */
				memcpy(svbufp->buf+db_oldsysvar[DB_QDIMSTYLE].bc,&fshort1,2);
				/* P_UCSNAME: */
				fshort1=-1;  /* "" */
				memcpy(svbufp->buf+db_oldsysvar[DB_QP_UCSNAME].bc,&fshort1,2);
				/* UCSNAME: */
				fshort1=-1;  /* "" */
				memcpy(svbufp->buf+db_oldsysvar[DB_QUCSNAME].bc,&fshort1,2);
			}
		}

		for (vidx=0; vidx<db_nvars; vidx++) {
			/* Ignore special-source (true read-only) vars -- don't give */
			/* them a field or try to set a default: */
			if (db_oldsysvar[vidx].spl&IC_SYSVARFLAG_SPECIALSOURCE) continue;

			// Skip over variables that are in object dictionaries - they don't exist yet...
			if (db_oldsysvar[vidx].spl&IC_SYSVARFLAG_OBJDICTONARY) continue;

			if (pass) {  /* Set defaults: */
				/*
				** Skip if this var is for some other buffer, or if
				** there IS no buffer:
				*/
				if (db_oldsysvar[vidx].loc!=type || svbufp->buf==NULL) continue;

				/* Delete old rb string and set up new default val: */
				if (rb.restype==RTSTR) delete [] rb.resval.rstring;
				rb.resval.rstring=NULL;
				rb.restype=db_oldsysvar[vidx].type;
				fi1=db_str2rb(db_oldsysvar[vidx].defval,&rb);

//Spirin Vitaly{
//To set the correct DWGCODEPAGE = GetACP()
				if (vidx == DB_QDWGCODEPAGE)
				{
					char	str_ansi[33]="";
					char	str_code[33]="";
					strcpy(str_ansi, "ANSI_");
					itoa(GetACP(), str_code, 10);
					strcat(str_ansi, str_code);
					fi1=db_str2rb(str_ansi, &rb);
				}
//Spirin Vitaly}
				/* Use setvar to set the default: */
				if (!fi1) db_setvar(NULL,vidx,&rb,&dummydraw,
					(type==1) ? svbufp : NULL,(type==2) ? svbufp : NULL,0);
			} else {  /* Measure the sizes (and set the .bc members): */
				if (db_oldsysvar[vidx].loc) {  /* CONFIG or SESSION only */
					db_oldsysvar[vidx].bc=db_sysvarbufsz[db_oldsysvar[vidx].loc];
					db_sysvarbufsz[db_oldsysvar[vidx].loc]+=db_oldsysvar[vidx].nc;
				} else {  /* HEADER; don't touch the .bc settings! */
					if ((fi1=db_oldsysvar[vidx].bc+db_oldsysvar[vidx].nc)>
						db_sysvarbufsz[0]) db_sysvarbufsz[0]=fi1;
				}
			}
		}  /* End for each var */
	}  /* End for each pass */

out:
	if (!rc && type>-1 && type<ntypes)
		rc=db_sysvarbufsz[type]; /* If it worked; set rc to the size. */

	if (rb.restype==RTSTR)
		{ delete [] rb.resval.rstring; rb.resval.rstring=NULL; }

	/* Don't let ~db_charbuflink() free this; it belongs to svbufp!: */
	dummydraw.header.buf=NULL; dummydraw.header.size=0;

	return rc;
}


/*F*/
/*
**	DB_IS_TABRECTYPE didn't make it as a macro because it uses
**	global array db_hitypeinfo[] (which can't be exported outside
**	of the db dll, apparently, but who knows why).
**	S0 -- I turned all the DB_IS_ macros into functions.
*/
DB_API int db_is_validhitype(int typ) { return (typ>=0 && typ<DB_NHITYPES); }
DB_API int db_is_tabrectype(int typ) {
	return (db_is_validhitype(typ) && db_hitypeinfo[typ].typetype==DB_TABLE);
}
DB_API int db_is_objecttype(int typ) {
	return (db_is_validhitype(typ) && db_hitypeinfo[typ].typetype==DB_OBJECT);
}
DB_API int db_is_subentitytype(int typ) {
	return (typ==DB_VERTEX || typ==DB_SEQEND || typ==DB_ATTRIB);
}
DB_API int db_is_entitytype(int typ)
	{
	return (db_is_validhitype(typ) && db_hitypeinfo[typ].typetype==DB_ENTITY);
	}


/*F*/
db_handitem *db_makehip(int hiptype, db_drawing *dp, char *tabrecname) {
/*
**	Allocates an object of type hiptype and returns a base
**	class (db_handitem) pointer to it.	dp and/or tabrecname may be NULL,
**	in which case the default (void) constructor may be used.
*/
	int usedef;
	db_handitem *newhip=NULL;


	usedef=1;
	if (db_is_tabrectype(hiptype)) {
		if (dp!=NULL && tabrecname!=NULL && *tabrecname) usedef=0;
	} else if (!db_is_objecttype(hiptype)) {
		if (dp!=NULL) usedef=0;
	}

	switch (hiptype) {
		case DB_APPIDTABREC 	 : newhip=(usedef) ? (new db_appidtabrec	  ) : (new db_appidtabrec(	 tabrecname,dp)); break;
		case DB_BLOCKTABREC 	 : newhip=(usedef) ? (new db_blocktabrec	  ) : (new db_blocktabrec(	 tabrecname,dp)); break;
		case DB_DIMSTYLETABREC	 : newhip=(usedef) ? (new db_dimstyletabrec   ) : (new db_dimstyletabrec(tabrecname,dp)); break;
		case DB_LAYERTABREC 	 : newhip=(usedef) ? (new db_layertabrec	  ) : (new db_layertabrec(	 tabrecname,dp)); break;
		case DB_LTYPETABREC 	 : newhip=(usedef) ? (new db_ltypetabrec	  ) : (new db_ltypetabrec(	 tabrecname,dp)); break;
		case DB_STYLETABREC 	 : newhip=(usedef) ? (new db_styletabrec	  ) : (new db_styletabrec(	 tabrecname,dp)); break;
		case DB_UCSTABREC		 : newhip=(usedef) ? (new db_ucstabrec		  ) : (new db_ucstabrec(	 tabrecname,dp)); break;
		case DB_VIEWTABREC		 : newhip=(usedef) ? (new db_viewtabrec 	  ) : (new db_viewtabrec(	 tabrecname,dp)); break;
		case DB_VPORTTABREC 	 : newhip=(usedef) ? (new db_vporttabrec	  ) : (new db_vporttabrec(	 tabrecname,dp)); break;
		case DB_VXTABREC		 : newhip=(usedef) ? (new db_vxtabrec		  ) : (new db_vxtabrec( 	 tabrecname,dp)); break;

		case DB_3DFACE			 : newhip=(usedef) ? (new db_3dface 		  ) : (new db_3dface(			dp)); break;
		case DB_3DSOLID 		 : newhip=(usedef) ? (new db_3dsolid		  ) : (new db_3dsolid(			dp)); break;
		case DB_ACAD_PROXY_ENTITY: newhip=(usedef) ? (new db_acad_proxy_entity) : (new db_acad_proxy_entity(dp)); break;
		case DB_ARC 			 : newhip=(usedef) ? (new db_arc			  ) : (new db_arc(				dp)); break;
		case DB_ATTDEF			 : newhip=(usedef) ? (new db_attdef 		  ) : (new db_attdef(			dp)); break;
		case DB_ATTRIB			 : newhip=(usedef) ? (new db_attrib 		  ) : (new db_attrib(			dp)); break;
		case DB_BLOCK			 : newhip=(usedef) ? (new db_block			  ) : (new db_block(			dp)); break;
		case DB_BODY			 : newhip=(usedef) ? (new db_body			  ) : (new db_body( 			dp)); break;
		case DB_CIRCLE			 : newhip=(usedef) ? (new db_circle 		  ) : (new db_circle(			dp)); break;
		case DB_DIMENSION		 : newhip=(usedef) ? (new db_dimension		  ) : (new db_dimension(		dp)); break;
		case DB_ELLIPSE 		 : newhip=(usedef) ? (new db_ellipse		  ) : (new db_ellipse(			dp)); break;
		case DB_ENDBLK			 : newhip=(usedef) ? (new db_endblk 		  ) : (new db_endblk(			dp)); break;
		case DB_HATCH			 : newhip=(usedef) ? (new db_hatch			  ) : (new db_hatch(			dp)); break;
		case DB_IMAGE			 : newhip=(usedef) ? (new db_image			  ) : (new db_image(			dp)); break;
		case DB_INSERT			 : newhip=(usedef) ? (new db_insert 		  ) : (new db_insert(			dp)); break;
		case DB_LEADER			 : newhip=(usedef) ? (new db_leader 		  ) : (new db_leader(			dp)); break;
		case DB_LINE			 : newhip=(usedef) ? (new db_line			  ) : (new db_line( 			dp)); break;
		case DB_LWPOLYLINE		 : newhip=(usedef) ? (new db_lwpolyline 	  ) : (new db_lwpolyline(		dp)); break;
		case DB_MLINE			 : newhip=(usedef) ? (new db_mline			  ) : (new db_mline(			dp)); break;
		case DB_MTEXT			 : newhip=(usedef) ? (new CDbMText			  ) : (new CDbMText(			dp)); break;
		case DB_OLE2FRAME		 : newhip=(usedef) ? (new db_ole2frame		  ) : (new db_ole2frame(		dp)); break;
		case DB_POINT			 : newhip=(usedef) ? (new db_point			  ) : (new db_point(			dp)); break;
		case DB_POLYLINE		 : newhip=(usedef) ? (new db_polyline		  ) : (new db_polyline( 		dp)); break;
		case DB_RAY 			 : newhip=(usedef) ? (new db_ray			  ) : (new db_ray(				dp)); break;
		case DB_REGION			 : newhip=(usedef) ? (new db_region 		  ) : (new db_region(			dp)); break;
		case DB_SEQEND			 : newhip=(usedef) ? (new db_seqend 		  ) : (new db_seqend(			dp)); break;
		case DB_SHAPE			 : newhip=(usedef) ? (new db_shape			  ) : (new db_shape(			dp)); break;
		case DB_SOLID			 : newhip=(usedef) ? (new db_solid			  ) : (new db_solid(			dp)); break;
		case DB_SPLINE			 : newhip=(usedef) ? (new db_spline 		  ) : (new db_spline(			dp)); break;
		case DB_TEXT			 : newhip=(usedef) ? (new db_text			  ) : (new db_text( 			dp)); break;
		case DB_TOLERANCE		 : newhip=(usedef) ? (new db_tolerance		  ) : (new db_tolerance(		dp)); break;
		case DB_TRACE			 : newhip=(usedef) ? (new db_trace			  ) : (new db_trace(			dp)); break;
		case DB_VERTEX			 : newhip=(usedef) ? (new db_vertex 		  ) : (new db_vertex(			dp)); break;
		case DB_VIEWPORT		 : newhip=(usedef) ? (new db_viewport		  ) : (new db_viewport( 		dp)); break;
		case DB_XLINE			 : newhip=(usedef) ? (new db_xline			  ) : (new db_xline(			dp)); break;

		case DB_DICTIONARY		 : newhip=new db_dictionary 	  ; break;
		case DB_DICTIONARYVAR	 : newhip=new db_dictionaryvar	  ; break;
		case DB_GROUP			 : newhip=new CDbGroup			  ; break;
		case DB_IDBUFFER		 : newhip=new db_idbuffer		  ; break;
		case DB_IMAGEDEF		 : newhip=new db_imagedef		  ; break;
		case DB_IMAGEDEF_REACTOR : newhip=new db_imagedef_reactor ; break;
		case DB_MLINESTYLE		 : newhip=new db_mlinestyle 	  ; break;
		case DB_ACAD_PROXY_OBJECT: newhip=new db_acad_proxy_object; break;
		case DB_RASTERVARIABLES  : newhip=new db_rastervariables  ; break;
		case DB_SPATIAL_FILTER	 : newhip=new db_spatial_filter   ; break;
		case DB_XRECORD 		 : newhip=new db_xrecord		  ; break;
		case DB_PLOTSETTINGS	 : newhip=new CDbPlotSettings	  ; break;
		case DB_LAYOUT			 : newhip=new CDbLayout 		  ; break;
		case DB_PLACEHOLDER 	 : newhip=new db_placeholder	  ; break;
		case DB_DICTIONARYWDFLT  : newhip=new db_dictionarywdflt  ; break;
		case DB_WIPEOUTVARIABLES : newhip=new db_wipeoutvariables ; break;
		case DB_VBAPROJECT		 : newhip=new db_vbaproject 	  ; break;
		case DB_LAYER_INDEX 	 : newhip=new db_layer_index	  ; break;
		case DB_OBJECT_PTR		 : newhip=new db_object_ptr 	  ; break;
		case DB_SORTENTSTABLE	 : newhip=new CDbSortentsTable	  ; break;
		case DB_SPATIAL_INDEX	 : newhip=new db_spatial_index	  ; break;
		case DB_DIMASSOC		 : newhip=new db_dimassoc		  ; break;	// 2002/7/10 EBATECH(CNBR) Bugzilla#78218
	}

	return newhip;
}

//create proper handitem and assign values from resbuf.
DB_API db_handitem* db_makehipfromrb(resbuf *rb, db_drawing *drawing)
{

	if(ic_assoc(rb,0)!=0)
		return NULL;

	db_handitem *newEnt = db_makehip(db_str2hitype(ic_rbassoc->resval.rstring), drawing, NULL);
	newEnt->entmod(rb, drawing);

	return newEnt;
}


char *db_makeanonname(char *sour, db_drawing *dp, int donotdisturb) {
/*
**	Allocates a string (via "new") and returns a pointer to it --
**	unless sour==NULL (in which case NULL is returned).
**	(The caller must eventually "delete" the returned pointer.)
**
**	If sour indicates an anonymous block name, the new string
**	is the completed name (with the number attached).  Otherwise,
**	it's the same as sour.
**
**	When donotdisturb is non-0, this function will NOT generate
**	a new anonymous name if sour appears to already HAVE a
**	valid anonymous name (i.e., already has a number).
**	(WE AREN'T USING THIS FEATURE YET.	ALL CALLS HAVE donotdisturb==0.
**	WE MAY USE IT SOMEDAY -- BUT LOOK OUT IN THE DRW CODE!
*/
	/*D.G.*/// It's not true now: The call from db_blocktabrec::set_2 has donotdisturb == 1.

	char *ret,num[21],fc1;
	int fi1;


	ret=NULL;

	if (sour!=NULL)
	{
		// Added strnicmp("*MODEL_SPACE",sour,12) != 0 because my block name was comming out *Uodel_space. MHB
		if (strnicmp("*PAPER_SPACE",sour,12) && strnicmp("*MODEL_SPACE",sour,12)) // if it DOES match, don't do this number-attaching
		{
			if (*sour=='*'/*DNT*/ && sour[1] && dp!=NULL)	/* At least a normal anonymous name */
			{
				fi1=(sour[2]=='|' &&
					toupper(sour[3])=='R'/*DNT*/ &&
					toupper(sour[4])=='E'/*DNT*/ &&
					toupper(sour[5])=='F'/*DNT*/) ? 6 : 2;	/* Where the number will go. */
												 /* (2 : Normal; 6 : Xref */
			/*
			**	If there's nothing already there, or it's not a number,
			**	or we don't care if it is, then generate the next
			**	anonymous name:
			*/
				if (!sour[fi1] || !isdigit((unsigned char) sour[fi1]) || !donotdisturb)
				{
					char *blockNum;
					int  blockNumCount = 0,
						 sourCount;
					
					for(sourCount = fi1 ; sour[sourCount] != '\0' ; sourCount++);

					blockNum  = new char[sourCount];
					sourCount = fi1; 
					
					while(sour[sourCount])
					{
						if(isdigit((unsigned char) sour[sourCount]))
							blockNum[blockNumCount++] = sour[sourCount++];
						else 
						{
							blockNumCount = 0;
							break;
						}
					}
					blockNum[blockNumCount] = '\0';
					
					sprintf(num,"%d"/*DNT*/,dp->use_nextanon(atoi(blockNum)));
					delete [] blockNum;
					ret=new char[fi1+strlen(num)+1];
					fc1=sour[fi1]; sour[fi1]=0;
					strcpy(ret,sour);
				   sour[fi1]=fc1;
					strcpy(ret+fi1,num);
				}
// any non-D or X or B type anon blocks should be made as *U type
/*D.G.*/// 'B' code added for compatibility with German ACAD (CR1606).
				// EBATECH(CNBR) 'E' code added for Explofing non-uniform scale inserts. 2003/4/30
				//if( toupper(sour[1]) != 'D' && toupper(sour[1]) != 'X' && toupper(sour[1]) != 'B')
				if(strchr("TDXBEtdxbe", sour[1]) == NULL )
				{
					sour[1]='U';
				}
			}
		}

		if (ret==NULL && strcmp(sour,"*"))
			{
			ret=new char[strlen(sour)+1];
			strcpy(ret,sour);
			}
	}

	return ret;
}


/********** 	   HELPER FUNCTIONS FOR DIMENSIONS	**********/


/*F*/
DB_API int db_ucshelper(
	sds_point  ucsorg,
	sds_point *ucsaxis,
	sds_real   elev,
	sds_point  ecsinspt,
	sds_point  wcsinspt,
	sds_real  *protp) {
/*
**
**	This was written as an aid for creating DIMENSIONS in a given
**	UCS at a given elevation, but it may have other applications, too.
**	Data passed to it may be for the current UCS -- or for some other
**	UCS (like the UCS an existing DIMENSION was created in).
**
**	Given:
**
**	   ucsorg : The UCS origin to use (WCS coordinates).
**
**	  ucsaxis : The UCS axes to use. (WCS coordinates.	[0]=X; [1]=Y; [2]=Z.)
**				  Must be an array of at least 3 sds_points.
**
**		 elev : The elevation to use in the specified UCS.
**
**	  NOTE: If you're using an array of 4 points (call it ap[]) to
**	  define the UCS, where ap[0] is the origin and ap[1] through
**	  ap[3] are the axes (like dimucs in db_initdimucs()), pass
**	  ap[0] as ucsorg and ap+1 as ucsaxis.
**
**	Set by this function:
**
**	  ecsinspt : The ECS point for the insertion of a DIMENSION
**				   anonymous block.  This is (0,0,ecselev), where
**				   ecselev is the elevation according to the ECS
**				   defined by the UCS's Z-axis.  (Use this one when
**				   you do a set_12() for a DIMENSION.)
**
**	  wcsinspt : The WCS equivalent of ecsinspt.  (Use this one
**				   for the 12-group when you entmake/mod DIMENSIONs.)
**
**		 protp : *protp will be set to the rotation a POINT entity
**				   would have if created in the specified UCS (not in a
**				   block def). This is the angle offset of the ECS
**				   defined by the Z-axis of the specified UCS
**				   from the specified UCS, measured CCW in the
**				   specified UCS.  (Use this value for the 50-groups
**				   in the POINT entities in DIMENSION block defs (even
**				   though this is the rotation a POINT would have if
**				   NOT in a block def), and for the 51-group in
**				   DIMENSIONs.)
**
**	For some stupid reason, the insertion pt (12-group) is stored
**	in a DWG file as ECS but communicated to the user (via entget/make/mod)
**	as the corresponding WCS point.  That's why we have ecsinspt
**	for the set_12() and wcsinspt for entmake/mod.
**
**	Returns:
**		 0 : OK
**		-1 : ucsaxis is NULL.
**		-2 : Invalid UCS specified (ucsaxis[2] is (0,0,0)).
*/
	int rc;
	sds_real wprot;
	sds_point ecsaxis[3],wecsinspt,wwcsinspt,ap1;


	rc=0;

	wprot=0.0;
	wecsinspt[0]=wecsinspt[1]=wecsinspt[2]=0.0;
	wwcsinspt[0]=wwcsinspt[1]=wwcsinspt[2]=0.0;

	// set input parameter to default value
	//
	*protp = 0.0;

	if (ucsaxis==NULL) { rc=-1; goto out; }
	if (icadRealEqual(ucsaxis[2][0],0.0) &&
		icadRealEqual(ucsaxis[2][1],0.0) &&
		icadRealEqual(ucsaxis[2][2],0.0)) { rc=-2; goto out; }

	/* Get the ecs defined by the UCS's Z-axis: */
	DB_PTCPY(ecsaxis[2],ucsaxis[2]);
	ic_arbaxis(ecsaxis[2],ecsaxis[0],ecsaxis[1],ecsaxis[2]);


	/*
	**	wprotp:
	**
	**	ecsaxis[0] is the ECS X-axis according to the WCS.	Transform it
	**	to the UCS and use atan2(ycomponent,xcomponent) to get wprot.
	*/
	wprot=atan2(ecsaxis[0][0]*ucsaxis[1][0]+
				ecsaxis[0][1]*ucsaxis[1][1]+
				ecsaxis[0][2]*ucsaxis[1][2],
				ecsaxis[0][0]*ucsaxis[0][0]+
				ecsaxis[0][1]*ucsaxis[0][1]+
				ecsaxis[0][2]*ucsaxis[0][2]);
	ic_normang(&wprot,NULL);

	/*
	**	wecsinspt:
	**
	**	If we transform a UCS point at elevation elev (0,0,elev)
	**	to the ECS, the Z-component will be the elevation
	**	according to the ECS (?,?,ecselev).  Then ecsinspt will be
	**	(0,0,ecselev).
	*/
	/* UCS->WCS: */
	ap1[0]=elev*ucsaxis[2][0]+ucsorg[0];
	ap1[1]=elev*ucsaxis[2][1]+ucsorg[1];
	ap1[2]=elev*ucsaxis[2][2]+ucsorg[2];
	/* WCS->ECS (Z-component only): */
	wecsinspt[2]=ap1[0]*ecsaxis[2][0]+
				 ap1[1]*ecsaxis[2][1]+
				 ap1[2]*ecsaxis[2][2];
	/* Complete the wecsinspt: */
	wecsinspt[0]=wecsinspt[1]=0.0;

	/*
	**	wwcsinspt:
	**
	**	Just transform wecsinspt ECS->WCS.	(Remember: wecsinspt[0] and
	**	wecsinspt[1] are 0.0.)
	*/
	wwcsinspt[0]=wecsinspt[2]*ecsaxis[2][0];
	wwcsinspt[1]=wecsinspt[2]*ecsaxis[2][1];
	wwcsinspt[2]=wecsinspt[2]*ecsaxis[2][2];

out:
	if (!rc) {
		if (ecsinspt!=NULL) DB_PTCPY(ecsinspt,wecsinspt);
		if (wcsinspt!=NULL) DB_PTCPY(wcsinspt,wwcsinspt);
		if (protp!=NULL) *protp=wprot;
	}
	return rc;
}

/*F*/
DB_API void db_initdimucs(
	sds_point	*dimucs,
	sds_real	*dimelevp,
	db_drawing	*dp,
	db_handitem *dimhip) {
/*
**	This function was written specifically for working with the
**	creation or modification of DIMENSIONs.  Once set up by this function,
**	dimucs[] can be passed to db_drawing::trans() to be used in place
**	of the CURRENT UCS.
**
**	Sets up dimucs[] so that it defines the "creation UCS" for the DIMENSION
**	specified by dimhip (when dimhip!=NULL).  If you want it to use
**	the CURRENT UCS because that's where the DIMENSION is being created
**	by the user, pass dimhip as NULL.
**
**	*dimelevp is set to the elevation for the dimension in the dimucs
**	system.  When dimhip is NULL, the current elevation is used.
**	Otherwise, it is set to the elevation that the 12-pt implies
**	for the dimucs system that is set up.
**
**	dimucs[] must be an array of at least 4 sds_points.  It will be set up
**	as follows (all coordinates being WCS):
**
**	  [0] : The UCS origin.
**	  [1] : The UCS X-direction unit vector.
**	  [2] : The UCS Y-direction unit vector.
**	  [3] : The UCS Z-direction unit vector.
**
**	The CURRENT UCS part of this function is adapted from gr_getucs().
*/
	char *headerbuf;
	int inps,usedefaults,fi1;
	sds_real cs,sn,ar1;
	sds_point ap1,ap2;
	struct resbuf rb;


	usedefaults=1; inps=0; memset(&rb,0,sizeof(rb));

	if (dimucs==NULL) goto out;

	if (dimhip==NULL) {  /* Use CURRENT UCS. */

		if (dp==NULL || (headerbuf=dp->ret_headerbuf())==NULL) goto out;

		inps=dp->inpspace();

		db_qgetvar((inps) ? DB_QP_UCSORG  : DB_QUCSORG ,headerbuf,dimucs[0],DB_3DPOINT,0);
		db_qgetvar((inps) ? DB_QP_UCSXDIR : DB_QUCSXDIR,headerbuf,dimucs[1],DB_3DPOINT,0);
		db_qgetvar((inps) ? DB_QP_UCSYDIR : DB_QUCSYDIR,headerbuf,dimucs[2],DB_3DPOINT,0);

		/* Get the unit UCS axes: */
		for (fi1=1; fi1<3; fi1++) {  /* Unitize the X- and Y-axes. */
			if (icadRealEqual((ar1=DB_VLEN(dimucs[fi1])),0.0)) goto out;
			if (!icadRealEqual((ar1=1.0/ar1),1.0)) DB_VSCALE(dimucs[fi1],ar1);
		}

		/* Cross X with Y to get Z-axis: */
		dimucs[3][0]=dimucs[1][1]*dimucs[2][2]-dimucs[1][2]*dimucs[2][1];
		dimucs[3][1]=dimucs[1][2]*dimucs[2][0]-dimucs[1][0]*dimucs[2][2];
		dimucs[3][2]=dimucs[1][0]*dimucs[2][1]-dimucs[1][1]*dimucs[2][0];
		if ((ar1=DB_VLEN(dimucs[3]))<IC_ZRO) goto out;
		if ((ar1=1.0/ar1)!=1.0) DB_VSCALE(dimucs[3],ar1);

		/* Cross Z with X to make SURE Y-axis is normal to the others: */
		dimucs[2][0]=dimucs[3][1]*dimucs[1][2]-dimucs[3][2]*dimucs[1][1];
		dimucs[2][1]=dimucs[3][2]*dimucs[1][0]-dimucs[3][0]*dimucs[1][2];
		dimucs[2][2]=dimucs[3][0]*dimucs[1][1]-dimucs[3][1]*dimucs[1][0];

		if (dimelevp!=NULL) {
			/* Use the current elevation.  (Use db_getvar() this time */
			/* (not db_qgetvar()) because we need the WCS to UCS transformation.) */
			db_getvar(NULL,(inps) ? DB_QP_ELEVATION : DB_QELEVATION,
				&rb,dp,NULL,NULL);
			*dimelevp=rb.resval.rreal;
		}

	} else {  /* Use dimhip to set dimucs[]. */

		if (dimhip->ret_type()!=DB_DIMENSION) goto out;

		/* Set the Z-axis from the extrusion vector: */
		dimhip->get_210(dimucs[3]);

		/* Start with the ECS system: */
		ic_arbaxis(dimucs[3],dimucs[1],dimucs[2],dimucs[3]);

	  /* dimucs[1-3] ARE THE ECS AXES UNTIL ROTATED LATER. */

		/*
		**	I think we get to choose a UCS origin, as long as we've got
		**	the correct axis vectors and set *dimelevp properly.
		**	I don't see a way to determine it from the DIMENSION entity data.
		**	Let's choose (0,0,0):
		*/
		dimucs[0][0]=dimucs[0][1]=dimucs[0][2]=0.0;

		if (dimelevp!=NULL) {
			/*
			**	The 12-pt is the ECS insertion pt (ECS in the database).
			**	Set *dimelev to the Z-coordinate:
			*/
			dimhip->get_12(ap1);
			*dimelevp=ap1[2]; /* ECS-elevation */

			/*
			**	Because we're choosing (0,0,0) as the UCS origin, the
			**	ECS-elevation is also the UCS-elevation.  But, in case we
			**	set it to something else someday, let's go ahead and
			**	determine the UCS-elevation properly.  We transform
			**	the UCS origin to the ECS and then subtract its Z-coordinate
			**	from the ECS-elevation.  (For a UCS origin of (0,0,0),
			**	this sets the UCS-elevation equal to the ECS one.)
			*/
			*dimelevp-=dimucs[0][0]*dimucs[3][0]+
					   dimucs[0][1]*dimucs[3][1]+
					   dimucs[0][2]*dimucs[3][2];
		}

		/*
		**	The ECS's angular offset from the UCS is the 51-group.
		**	Rotate the ECS X- and Y-axes CW about the Z-axis by that amount
		**	to get the UCS axes that correspond to this DIMENSION.	(For the
		**	X-axis, it's cs*(current X-vector)-sn*(current Y-vector).
		**	For the Y-axis, it's cs*(current Y-vector)+
		**	sn*(current X-vector).)
		*/
		dimhip->get_51(&ar1); cs=cos(ar1); sn=sin(ar1);
		ap1[0]=cs*dimucs[1][0]-sn*dimucs[2][0];
		ap1[1]=cs*dimucs[1][1]-sn*dimucs[2][1];
		ap1[2]=cs*dimucs[1][2]-sn*dimucs[2][2];
		ap2[0]=cs*dimucs[2][0]+sn*dimucs[1][0];
		ap2[1]=cs*dimucs[2][1]+sn*dimucs[1][1];
		ap2[2]=cs*dimucs[2][2]+sn*dimucs[1][2];
		DB_PTCPY(dimucs[1],ap1); DB_PTCPY(dimucs[2],ap2);

	  /* NOW dimucs[] DESCRIBES THE DIMENSION UCS. */

	}

	usedefaults=0;

out:
	if (usedefaults) {
		if (dimucs!=NULL) {
			memset(dimucs,0,4*sizeof(sds_point));
			dimucs[1][0]=dimucs[2][1]=dimucs[3][2]=1.0;
		}
		if (dimelevp!=NULL) *dimelevp=0.0;
	}
}

/*
 *	Julian to Gregorian date conversion via ODA conversion utility
 */
void gregorian(long j,short *d,short *m,short *y)
{
	long td;

	j -= 1721119L;
	*y = (short)((4L*j-1L) / 146097L);
	j = 4*j-1-146097*(*y);
	td = j / 4;
	j = (4*td+3) / 1461;
	td = 4*td+3-1461*j;
	td = (short)((td+4) / 4);
	*m = (short)((5*td-3) / 153);
	td = 5*td-3-153*(*m);
	*d = (short)((td+5) / 5);
	*y = (short)(100*(*y)+j);
	if (*m<10) 
		*m+=3;
	else 
	{ 
		*m-=9; 
		(*y)++; 
	}

	return;
}

void convertAcadTime(long secl,short *hour,short *mins,short *sec,short *milliseconds)
{
	long seclong;
	double secfl;

	*milliseconds = (short)(secl % 1000L);
	secfl = (double)secl/1000.0+0.5;
	seclong = (long)secfl;
	*hour = (short)(seclong / 3600);
	seclong -= 3600*(long)(*hour);
	*mins = (short)(seclong / 60);
	seclong -= 60*(*mins);
	*sec = (short)seclong;

	return;
}

void decodeAcadDate(const long *longs,short *day,short *month,
                      short *year,short *hour,short *mins,short *sec)
{
  short milliseconds;

  if (longs[0]==0L && longs[1]==0L) {
    *day=1; *month=1; *year=1990;
    *hour=*mins=*sec=0;
    return;
  }
  gregorian(longs[0],day,month,year);              /* sets these values */
                                                /* note they are pointers */
  convertAcadTime(longs[1],hour,mins,sec,&milliseconds);
}


DB_API void db_jul2greg(double dJul,short *pYear,short *pMonth,short *pDay,short *pDow,short *pHour,short *pMin,double *pSec)
	{
	long	date[2];

		// convert to AutoCAD format: long Julian Day and long milliseconds
	date[0] = (long)dJul;
	date[1] = (long)((dJul - date[0]) * 24 * 60 * 60 * 1000 +.5);

	short seconds = 0;
	decodeAcadDate( date, pDay, pMonth, pYear, pHour, pMin, &seconds);

	*pDow = (short)((date[0]+1) % 7);
	*pSec = seconds + (date[1] % 1000)/1000.;
	}


DB_API int db_samedirection(sds_point v0, sds_point v1) {
/*
**	Determines whether or not vectors v0 and v1 are in the same
**	DIRECTION, within a tolerance.	(They can differ in length
**	considerably, however.)
**
**	Examines the length of the delta of the unit vectors.
**	(The dot product cosine trick isn't as good because we deal with
**	the square of a length that is close to 1.0.  We'd need 30
**	digits of accuracy to get a 15 digit result.  In contrast,
**	proximity to 0.0 is no problem.)
**
**	Returns 1 for YES and 0 for NO.
*/
	int rc;
	sds_real len0,len1;
	sds_point wv0,wv1;

	rc=0;

	if (v0!=NULL && v1!=NULL) {
		DB_PTCPY(wv0,v0); DB_PTCPY(wv1,v1);
		len0=DB_VLEN(wv0); len1=DB_VLEN(wv1);
		if (len0>0.0 && len1>0.0) {
			if (len0!=1.0) { wv0[0]/=len0; wv0[1]/=len0; wv0[2]/=len0; }
			if (len1!=1.0) { wv1[0]/=len1; wv1[1]/=len1; wv1[2]/=len1; }
			wv0[0]-=wv1[0]; wv0[1]-=wv1[1]; wv0[2]-=wv1[2];
			if (DB_VLEN(wv0)<IC_ZRO) rc=1;
		}
	}

	return rc;
}


/**********  End of HELPER FUNCTIONS FOR DIMENSIONS  **********/

/**********  HELPER FUNCTIONS FOR AutoCAD 2002 Support	**********/

/*DG - 11.6.2002*/// This function is needed at least for layer's dxf370 setting which is supported since dxf 2000.

static int validweightlist[] = { 0, 5, 9, 13, 15, 18, 20, 25, 30, 35, 40, 50, 53, 60, 70, 80, 90, 100, 106, 120, 140, 158, 200, 211};


DB_API bool db_is_validweight(int weight, int bitcode )
/*
	validate lineweight value.
	(i) weight : validating value.
	(i) bitcode : Like linetype, Entity has 4 state Immediate, "ByBlock", "ByLayer", and "Default".
	   "Default" is depended by LWDEFAULT system variable.
	   Layer records has 2 state Immediate and "Default".
	   LWDEFAULT system variable has Immediate only.
	   So, this function accepts several validation conditions using bitcode.
	true : Okay / false : NoGood
*/
{
	int i, n;

	if( weight == DB_BLWEIGHT && ( bitcode & DB_BLWMASK ) != 0 ) // BYLAYER
		return true;
	else if( weight == DB_BBWEIGHT && ( bitcode & DB_BBWMASK ) != 0 ) // BYBLOCK
		return true;
	else if( weight == DB_DFWEIGHT && ( bitcode & DB_DFWMASK ) != 0 )	// DEFAULT
		return true;
	else if( 0 <= weight && weight <= 211 ){
		for( i = 0, n = sizeof(validweightlist) / sizeof(int); i < n ; i++ ){
			if( weight == validweightlist[i] ){
				return true;
			}
		}
	}
	return false;
}

DB_API int db_lweight2index( int weight )
{
	int i, n;

	if( weight == DB_BLWEIGHT || weight == DB_BBWEIGHT || weight == DB_DFWEIGHT )
		return weight;
	else if( 0 <= weight && weight <= 211 ){
		for( i = 0, n = sizeof(validweightlist) / sizeof(int); i < n ; i++ ){
			if( weight == validweightlist[i] ){
				return i;
			}
		}
	}
	ASSERT(FALSE);
	return DB_DFWEIGHT;
}

DB_API int db_index2lweight(int index )
{
	if( index == DB_BLWEIGHT || index == DB_BBWEIGHT || index == DB_DFWEIGHT )
		return index;
	else if( 0 <= index && index < sizeof(validweightlist) / sizeof(int))
		return validweightlist[index];
	ASSERT(FALSE);
	return DB_DFWEIGHT;
}

DB_API db_handitem* db_ret_default_plotstylehip(db_drawing* dp )
/*
	get default object pointer of plotstylename.
	Used by initialize of LAYER table record.
	NULL : error / pointer : succeed
*/
{
	db_handitem* retp;
	db_objhandle hand;

	if( dp )
	{
		hand = dp->ret_stockhand(DB_SHI_PLACEHOLDER);
		if(hand != NULL )
		{
			retp = dp->handent(hand);
			if( retp != NULL )
			{
				return retp;
			}
		}
	}
	return NULL;
}

DB_API bool db_current_plotstyle( db_drawing* dp, int* plotstyletype, db_handitem** retp )
/*
	get current plotstyletype int and/or object pointer of plotstylename.
	Used by initialize of all entity object.
	(i) dp : drawing pointer to get CPLOTSTYLE variable and ACAD_PLOTSTYLENAME dictionary.
	(o) plotstyletype : result(DXF=380), Entity has 3 value "ByLayer", "ByBlock", Immediate.
	(o) retp : result(DXF=390), If plotstyletype is Immediate, retp sets PLACEHOLDER object.
	(r) true : suceed / false : fail
*/
{
	bool ret = false;
	int plotstyle;
	char* cplotstyle = NULL;
	db_handitem *dicthip = NULL;
	db_handitem *thip1;
	db_objhandle hand;

	if( !dp || !plotstyletype || !retp )
	{
		return ret;
	}

	*plotstyletype = DB_BL_PLSTYLE;
	*retp = NULL;
	// read $PSTYLEMODE to check Bycolor drawing
	db_qgetvar(DB_QPSTYLEMODE, dp->ret_headerbuf(), &plotstyle, DB_INT, 0);
	if( plotstyle == DB_BYCOL_PSTYLEMODE )
	{
		return true;
	}
	// read $CPLOTSTYLE to decide current plotstyle
	db_qgetvar(DB_QCPLOTSTYLE, dp->ret_headerbuf(), &cplotstyle, DB_ASTRING, 0);
	hand = dp->ret_stockhand( DB_SHI_DICWDFLT );
	if( hand != NULL )
	{
		dicthip = dp->handent( hand );
	}

	if( cplotstyle && stricmp(cplotstyle, db_str_bylayer) == 0 )
	{
		*plotstyletype = DB_BL_PLSTYLE;
		ret = true;
	}
	else if( cplotstyle && stricmp(cplotstyle, db_str_byblock) == 0 )
	{
		*plotstyletype = DB_BB_PLSTYLE;
		ret = true;
	}
	else if( cplotstyle && dicthip && ( thip1 = dp->dictsearch( dicthip, cplotstyle, 0)) != NULL )
	{
		*plotstyletype = DB_NM_PLSTYLE;
		*retp = thip1;
		ret = true;
	}
	else
	{
		hand = dp->ret_stockhand( DB_SHI_PLACEHOLDER );
		if( hand != NULL && ( thip1 = dp->handent( hand )) != NULL )
	{
		*plotstyletype = DB_NM_PLSTYLE;
		*retp = thip1;
		ret = true;
	}
	}
	// free unused string...
	if( cplotstyle )
	{
		delete [] cplotstyle ;
	}
	return ret;
}

DB_API char* db_ret_plotstylename( db_handitem* hip, db_drawing* dp )
/*
	Convert from placeholder object to plotstylename.
	Used by LIST command, ENTPORPS command and more.
	(i) hip : pointer of placeholder object.
	(i) dp : drawing pointer to get ACAD_PLOTSTYLENAME dictionary.
	(r) string : plotstylename (don't free!) / NULL : error
*/
{
	int plotstyle;
	db_dictionarywdflt* dictp;
	db_dictionaryrec *recp;
	db_objhandle hand;

	db_qgetvar(DB_QPSTYLEMODE, dp->ret_headerbuf(), &plotstyle, DB_INT, 0);
	if( plotstyle == DB_BYCOL_PSTYLEMODE )
	{
		return db_str_bycolor;
	}
	hand = dp->ret_stockhand(DB_SHI_DICWDFLT);
	if( hand != NULL )
	{
		dictp = (db_dictionarywdflt*)( dp->handent( hand ));
	if( hip && dictp ){
		dictp->get_recllends(&recp, NULL);
		for(; recp ; recp = recp->next )
		{
				if( recp->hiref != NULL && recp->hiref->ret_hip(dp) == hip )
			{
				return recp->name;
				}
			}
		}
	}
	return NULL;
}
