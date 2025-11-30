/* C:\ICAD\PRJ\LIB\DB\HEADER.CPP
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "db.h"
#include "ad2.h"
#include "header_search_defs.h"
#include "header_search.h"

//compare functions for bsearch of the various DWG header variable types
// sleazy method -- just use one of the strus 'cuz we know they all start with char *
int headersearchcompare(const void *a,const void *b)
	{
	struct headershortsearchstru *a1,*b1;
	a1=(struct headershortsearchstru *)a;
	b1=(struct headershortsearchstru *)b;
	return strcmp(a1->name,b1->name);
	}


// ctors/dtors for search classes
headershortsearch::headershortsearch(void)
	{
	data=headershortsearchdata;
	numentries = sizeof(headershortsearchdata)   / sizeof(headershortsearchdata[0]);
	}

headershtstrsearch::headershtstrsearch(void)
	{
	data=headershtstrsearchdata;
	numentries = sizeof(headershtstrsearchdata)    / sizeof(headershtstrsearchdata[0]);
	}

headerrealsearch::headerrealsearch(void)
	{
	data=headerrealsearchdata;
	numentries = sizeof(headerrealsearchdata)   / sizeof(headerrealsearchdata[0]);
	}

headerpointsearch::headerpointsearch(void)
	{
	data=headerpointsearchdata;
	numentries = sizeof(headerpointsearchdata) / sizeof(headerpointsearchdata[0]);
	}

header3dpointsearch::header3dpointsearch(void)
	{
	data=header3dpointsearchdata;
	numentries = sizeof(header3dpointsearchdata)  / sizeof(header3dpointsearchdata[0]);
	}

headerstringsearch::headerstringsearch(void)
	{
	data=headerstringsearchdata;
	numentries = sizeof(headerstringsearchdata)  / sizeof(headerstringsearchdata[0]);
	}

headerhandlesearch::headerhandlesearch(void)
	{
	data=headerhandlesearchdata;
	numentries = sizeof(headerhandlesearchdata)     / sizeof(headerhandlesearchdata[0]);
	}

headerhipsearch::headerhipsearch(void)
	{
	data=headerhipsearchdata;
	numentries = sizeof(headerhipsearchdata)  / sizeof(headerhipsearchdata[0]);
	}


headershortsearch::~headershortsearch(void) {}
headershtstrsearch::~headershtstrsearch(void) {}
headerrealsearch::~headerrealsearch(void) {}
headerpointsearch::~headerpointsearch(void) {}
header3dpointsearch::~header3dpointsearch(void) {}
headerstringsearch::~headerstringsearch(void) {}
headerhandlesearch::~headerhandlesearch(void) {}
headerhipsearch::~headerhipsearch(void) {}

// record finds for search classes
struct headershortsearchstru *headershortsearch::FindRec(char *name)
	{
	struct headershortsearchstru shortkey;

	shortkey.name=name;
	return ((struct headershortsearchstru *)
		bsearch(&shortkey,data,numentries,sizeof(headershortsearch),headersearchcompare));
	}

struct headershtstrsearchstru *headershtstrsearch::FindRec(char *name)
	{
	struct headershtstrsearchstru shtstrkey;
	shtstrkey.name=name;
	return ((struct headershtstrsearchstru *)
		bsearch(&shtstrkey,data,numentries,sizeof(headershtstrsearch),headersearchcompare));
	}

struct headerrealsearchstru *headerrealsearch::FindRec(char *name)
	{
	struct headerrealsearchstru realkey;
	realkey.name=name;
	return ((struct headerrealsearchstru *)
		bsearch(&realkey,data,numentries,sizeof(headerrealsearch),headersearchcompare));
	}

struct headerpointsearchstru *headerpointsearch::FindRec(char *name)
	{
	struct headerpointsearchstru pointkey;
	pointkey.name=name;
	return ((struct headerpointsearchstru *)
		bsearch(&pointkey,data,numentries,sizeof(headerpointsearch),headersearchcompare));
	}

struct header3dpointsearchstru *header3dpointsearch::FindRec(char *name)
	{
	struct header3dpointsearchstru point3dkey;
	point3dkey.name=name;
	return ((struct header3dpointsearchstru *)
		bsearch(&point3dkey,data,numentries,sizeof(header3dpointsearch),headersearchcompare));
	}

struct headerstringsearchstru *headerstringsearch::FindRec(char *name)
	{
	struct headerstringsearchstru stringkey;
	stringkey.name=name;
	return ((struct headerstringsearchstru *)
		bsearch(&stringkey,data,numentries,sizeof(headerstringsearch),headersearchcompare));
	}

struct headerhandlesearchstru *headerhandlesearch::FindRec(char *name)
	{
	struct headerhandlesearchstru handlekey;
	handlekey.name=name;
	return ((struct headerhandlesearchstru *)
		bsearch(&handlekey,data,numentries,sizeof(headerhandlesearch),headersearchcompare));
	}

struct headerhipsearchstru *headerhipsearch::FindRec(char *name)
	{
	struct headerhipsearchstru hipkey;
	hipkey.name=name;
	return ((struct headerhipsearchstru *)
		bsearch(&hipkey,data,numentries,sizeof(headerhipsearch),headersearchcompare));
	}

// the search classes
headershortsearch hdrshortsearch;
headershtstrsearch hdrshtstrsearch;
headerrealsearch hdrrealsearch;
headerpointsearch hdrpointsearch;
header3dpointsearch hdr3dpointsearch;
headerstringsearch hdrstringsearch;
headerhandlesearch hdrhandlesearch;
headerhipsearch hdrhipsearch;


// Getvar -- Searches the various DWG header variable structures and retrieves the requested value.
// Translates values stored as handles, hips, or shtstrs (shorts that are passed in and out as strings) to
// strings.

short db_header::Getvar(db_drawing *dp, char *name,struct resbuf *rb)
	{
	struct headershortsearchstru *shortrec;
	struct headerrealsearchstru *realrec;
	struct headerpointsearchstru *pointrec;
	struct header3dpointsearchstru *point3drec;
	struct headerstringsearchstru *stringrec;
	struct headerhandlesearchstru *handlerec;
	struct headerhipsearchstru *hiprec;
	struct headershtstrsearchstru *shtstrrec;

	shortrec=hdrshortsearch.FindRec(name);
	if (shortrec!=NULL)
		{
		rb->resval.rint=(this->*(shortrec->retvarfn))(dp);	// and call the retvar member function (bizarre syntax, huh?)
		rb->restype=RTSHORT;							// set up the resbuf
		return RTNORM;									// and return
		}

	realrec=hdrrealsearch.FindRec(name);
	if (realrec!=NULL)
		{
		rb->resval.rreal=(this->*(realrec->retvarfn))(dp);	// and call the retvar member function
		rb->restype=RTREAL;								// set up the resbuf
		return RTNORM;									// and return
		}

	pointrec=hdrpointsearch.FindRec(name);
	if (pointrec!=NULL)
		{
		(this->*(pointrec->getvarfn))(dp,rb->resval.rpoint);	// and call the retvar member function
		rb->resval.rpoint[2]=0.0;
		rb->restype=RTPOINT;							// set up the resbuf
		return RTNORM;									// and return
		}

	point3drec=hdr3dpointsearch.FindRec(name);
	if (point3drec!=NULL)
		{
		(this->*(point3drec->getvarfn))(dp,rb->resval.rpoint);	// and call the retvar member function
		rb->restype=RT3DPOINT;							// set up the resbuf
		return RTNORM;									// and return
		}

	stringrec=hdrstringsearch.FindRec(name);
	if (stringrec!=NULL)
		{
		char *thestr=(this->*(stringrec->retvarfn))(dp);			// and call the retvar member function
		if ((rb->resval.rstring= new char [strlen(thestr)+1])==NULL)
			return RTERROR;
		strcpy(rb->resval.rstring,thestr);
		rb->restype=RTSTR;								// set up the resbuf
		return RTNORM;									// and return
		}

	handlerec=hdrhandlesearch.FindRec(name);
	if (handlerec!=NULL)
		{
		db_objhandle rethan=(this->*(handlerec->retvarfn))(dp);	// and call the retvar member function
		char hanbuf[17];
		rethan.ToAscii(hanbuf);
		if ((rb->resval.rstring= new char [strlen(hanbuf)+1])==NULL)
			return RTERROR;
		strcpy(rb->resval.rstring,hanbuf);
		rb->restype=RTSTR;								// set up the resbuf
		return RTNORM;									// and return
		}

	hiprec=hdrhipsearch.FindRec(name);
	if (hiprec!=NULL)
		{
		db_handitem *rethip=(this->*(hiprec->retvarfn))(dp);	// and call the retvar member function
		char namebuf[512];
		rethip->get_2(namebuf,512);
		if ((rb->resval.rstring= new char [strlen(namebuf)+1])==NULL)
			return RTERROR;
		strcpy(rb->resval.rstring,namebuf);
		rb->restype=RTSTR;								// set up the resbuf
		return RTNORM;									// and return
		}

	shtstrrec=hdrshtstrsearch.FindRec(name);
	if (shtstrrec!=NULL)
		{
		short sht=(this->*(shtstrrec->retvarfn))(dp);	// and call the retvar member function
		if (strsame(name,"DWGCODEPAGE"))				// as of this date (3/16/99), this the only shtstr
			{
			char codepagestr[60];
			adShortCodePageToAscii(sht,codepagestr);
			if ((rb->resval.rstring= new char [strlen(codepagestr)+1])==NULL)
				return RTERROR;
			strcpy(rb->resval.rstring,codepagestr);
			}
		rb->restype=RTSTR;								// set up the resbuf
		return RTNORM;									// and return
		}

	return RTERROR;			// not found
	}


// Setvar -- Searches the various DWG header variable structures and stores the given value.
// Translates values to be stored as handles, hips, or shtstrs (shorts that are passed in and out as strings) from
// strings to the appropriate type.  Performs range checks as appropriate.

short db_header::Setvar(db_drawing *dp, char *name,struct resbuf *rb)
	{
	struct headershortsearchstru *shortrec;
	struct headerrealsearchstru *realrec;
	struct headerpointsearchstru *pointrec;
	struct header3dpointsearchstru *point3drec;
	struct headerstringsearchstru *stringrec;
	struct headerhandlesearchstru *handlerec;
	struct headerhipsearchstru *hiprec;
	struct headershtstrsearchstru *shtstrrec;

	shortrec=hdrshortsearch.FindRec(name);
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
		(this->*(shortrec->setvarfn))(dp,rb->resval.rint);	// call the setvar member function (bizarre syntax, huh?)
		return RTNORM;									// and return
		}

	realrec=hdrrealsearch.FindRec(name);
	if (realrec!=NULL)
		{
		if (rb->restype!=RTREAL)
			return RTERROR;
		if (realrec->readonly)
			return RTERROR;
		if (realrec->hasmin && rb->resval.rreal<realrec->min)
			return RTERROR;
		if (realrec->hasmax && rb->resval.rreal>realrec->max)
			return RTERROR;
		(this->*(realrec->setvarfn))(dp,rb->resval.rreal);	// call the setvar member function
		return RTNORM;									// and return
		}

	pointrec=hdrpointsearch.FindRec(name);
	if (pointrec!=NULL)
		{
		if (rb->restype!=RTPOINT)
			return RTERROR;
		if (pointrec->readonly)
			return RTERROR;
		(this->*(pointrec->setvarfn))(dp,rb->resval.rpoint);	// call the setvar member function
		return RTNORM;										// and return
		}

	point3drec=hdr3dpointsearch.FindRec(name);
	if (point3drec!=NULL)
		{
		if (rb->restype!=RT3DPOINT)
			return RTERROR;
		if (point3drec->readonly)
			return RTERROR;
		(this->*(point3drec->setvarfn))(dp,rb->resval.rpoint);	// call the setvar member function
		return RTNORM;										// and return
		}

	stringrec=hdrstringsearch.FindRec(name);
	if (stringrec!=NULL)
		{
		if (rb->restype!=RTSTR)
			return RTERROR;
		if (stringrec->readonly)
			return RTERROR;
		(this->*(stringrec->setvarfn))(dp,rb->resval.rstring);	// call the setvar member function
		return RTNORM;										// and return
		}

	handlerec=hdrhandlesearch.FindRec(name);
	if (handlerec!=NULL)
		{
		if (rb->restype!=RTSTR)
			return RTERROR;
		if (handlerec->readonly)
			return RTERROR;
		db_objhandle han=rb->resval.rstring;
		(this->*(handlerec->setvarfn))(dp,han);				// call the setvar member function
		return RTNORM;										// and return
		}

	hiprec=hdrhipsearch.FindRec(name);
	if (hiprec!=NULL)
		{
		if (rb->restype!=RTSTR)
			return RTERROR;
		if (hiprec->readonly)
			return RTERROR;

		db_handitem *thehip=NULL;
		if (strisame(name,"CELTYPE"/*DNT*/))
			thehip=dp->findtabrec(DB_LTYPETAB,rb->resval.rstring,1);
		else if (strisame(name,"CLAYER"/*DNT*/))
			thehip=dp->findtabrec(DB_LAYERTAB,rb->resval.rstring,1);
		else if (strisame(name,"CMLSTYLE"/*DNT*/))
			{
			}
		else if (strisame(name,"DIMSTYLE"/*DNT*/))
			thehip=dp->findtabrec(DB_DIMSTYLETAB,rb->resval.rstring,1);
		else if (strisame(name,"DIMTXSTY"/*DNT*/))
			thehip=dp->findtabrec(DB_STYLETAB,rb->resval.rstring,1);
		else if (strisame(name,"P_UCSNAME"/*DNT*/))  // handle in UCSNAME
			thehip=dp->findtabrec(DB_UCSTAB,rb->resval.rstring,1);
		else if (strisame(name,"TEXTSTYLE"/*DNT*/))
			thehip=dp->findtabrec(DB_STYLETAB,rb->resval.rstring,1);
		else if (strisame(name,"UCSNAME"/*DNT*/))
			thehip=dp->findtabrec(DB_UCSTAB,rb->resval.rstring,1);

		if (thehip==NULL)
			return RTERROR;
		(this->*(hiprec->setvarfn))(dp,thehip);	// call the setvar member function
		return RTNORM;							// and return
		}

	shtstrrec=hdrshtstrsearch.FindRec(name);
	if (shtstrrec!=NULL)
		{
		if (shtstrrec->readonly)
			return RTERROR;
		if (shtstrrec->hasmin && rb->resval.rint<shtstrrec->min)
			return RTERROR;
		if (shtstrrec->hasmax && rb->resval.rint>shtstrrec->max)
			return RTERROR;
		if (strsame(name,"DWGCODEPAGE"))				// as of this date (3/16/99), this is the only shtstr
			{
			short sht;
			adAsciiCodePageToShort(rb->resval.rstring,&sht);
			(this->*(shtstrrec->setvarfn))(dp,sht);	// and call the retvar member function
			return RTNORM;									// and return
			}
		}

	return RTERROR;			// not found
	}



short db_header::RetAcisoutver(db_drawing *dp)
	{
	return acisoutver;
	}

short db_header::SetAcisoutver(db_drawing *dp, short a)
	{
	acisoutver=a;
	return RTNORM;
	}

short db_header::RetAflags(db_drawing *dp)
	{
	return aflags;
	}

short db_header::SetAflags(db_drawing *dp, short a)
	{
	aflags=a;
	return RTNORM;
	}

sds_real db_header::RetAngbase(db_drawing *dp)
	{
	return angbase;
	}

short db_header::SetAngbase(db_drawing *dp, sds_real a)
	{
	angbase=a;
	return RTNORM;
	}

short db_header::RetAngdir(db_drawing *dp)
	{
	return angdir;
	}

short db_header::SetAngdir(db_drawing *dp, short a)
	{
	angdir=a;
	return RTNORM;
	}

sds_real db_header::RetArea(db_drawing *dp)
	{
	return area;
	}

short db_header::SetArea(db_drawing *dp, sds_real a)
	{
	area=a;
	return RTNORM;
	}

short db_header::RetAttmode(db_drawing *dp)
	{
	return attmode;
	}

short db_header::SetAttmode(db_drawing *dp, short a)
	{
	attmode=a;
	return RTNORM;
	}

short db_header::RetAttreq(db_drawing *dp)
	{
	return attreq;
	}

short db_header::SetAttreq(db_drawing *dp, short a)
	{
	attreq=a;
	return RTNORM;
	}

short db_header::RetAunits(db_drawing *dp)
	{
	return aunits;
	}

short db_header::SetAunits(db_drawing *dp, short a)
	{
	aunits=a;
	return RTNORM;
	}

short db_header::RetAuprec(db_drawing *dp)
	{
	return auprec;
	}

short db_header::SetAuprec(db_drawing *dp, short a)
	{
	auprec=a;
	return RTNORM;
	}

short db_header::RetAxismode(db_drawing *dp)
	{
	return axismode;
	}

short db_header::SetAxismode(db_drawing *dp, short a)
	{
	axismode=a;
	return RTNORM;
	}

void db_header::GetAxisunit(db_drawing *dp, sds_point a)
	{
	a[0]=axisunit[0];
	a[1]=axisunit[1];
	a[2]=axisunit[2];
	}

short db_header::SetAxisunit(db_drawing *dp, sds_point a)
	{
	axisunit[0]=a[0];
	axisunit[1]=a[1];
	axisunit[2]=a[2];
	return RTNORM;
	}

sds_real db_header::RetBackz(db_drawing *dp)
	{
	return backz;
	}

short db_header::SetBackz(db_drawing *dp, sds_real a)
	{
	backz=a;
	return RTNORM;
	}

short db_header::RetBlipmode(db_drawing *dp)
	{
	return blipmode;
	}

short db_header::SetBlipmode(db_drawing *dp, short a)
	{
	blipmode=a;
	return RTNORM;
	}

short db_header::RetCecolor(db_drawing *dp)
	{
	return cecolor;
	}

short db_header::SetCecolor(db_drawing *dp, short a)
	{
	cecolor=a;
	return RTNORM;
	}

sds_real db_header::RetCeltscale(db_drawing *dp)
	{
	return celtscale;
	}

short db_header::SetCeltscale(db_drawing *dp, sds_real a)
	{
	celtscale=a;
	return RTNORM;
	}

db_handitem *db_header::RetCeltype(db_drawing *dp)
	{
	return celtype;
	}

short db_header::SetCeltype(db_drawing *dp, db_handitem *a)
	{
	celtype=a;
	return RTNORM;
	}

sds_real db_header::RetChamfera(db_drawing *dp)
	{
	return chamfera;
	}

short db_header::SetChamfera(db_drawing *dp, sds_real a)
	{
	chamfera=a;
	return RTNORM;
	}

sds_real db_header::RetChamferb(db_drawing *dp)
	{
	return chamferb;
	}

short db_header::SetChamferb(db_drawing *dp, sds_real a)
	{
	chamferb=a;
	return RTNORM;
	}

sds_real db_header::RetChamferc(db_drawing *dp)
	{
	return chamferc;
	}

short db_header::SetChamferc(db_drawing *dp, sds_real a)
	{
	chamferc=a;
	return RTNORM;
	}

sds_real db_header::RetChamferd(db_drawing *dp)
	{
	return chamferd;
	}

short db_header::SetChamferd(db_drawing *dp, sds_real a)
	{
	chamferd=a;
	return RTNORM;
	}

short db_header::RetChammode(db_drawing *dp)
	{
	return chammode;
	}

short db_header::SetChammode(db_drawing *dp, short a)
	{
	chammode=a;
	return RTNORM;
	}

sds_real db_header::RetCirclerad(db_drawing *dp)
	{
	return circlerad;
	}

short db_header::SetCirclerad(db_drawing *dp, sds_real a)
	{
	circlerad=a;
	return RTNORM;
	}

db_handitem *db_header::RetClayer(db_drawing *dp)
	{
	return clayer;
	}

short db_header::SetClayer(db_drawing *dp, db_handitem *a)
	{
	clayer=a;
	return RTNORM;
	}

short db_header::RetCmdactive(db_drawing *dp)
	{
	return cmdactive;
	}

short db_header::SetCmdactive(db_drawing *dp, short a)
	{
	cmdactive=a;
	return RTNORM;
	}

short db_header::RetCmdecho(db_drawing *dp)
	{
	return cmdecho;
	}

short db_header::SetCmdecho(db_drawing *dp, short a)
	{
	cmdecho=a;
	return RTNORM;
	}

char * db_header::RetCmdnames(db_drawing *dp)
	{
	return cmdnames;
	}

short db_header::SetCmdnames(db_drawing *dp, char *a)
	{
	if (cmdnames)
		delete cmdnames;
	db_astrncpy(&cmdnames,a,-1);
	return RTNORM;
	}

short db_header::RetCmljust(db_drawing *dp)
	{
	return cmljust;
	}

short db_header::SetCmljust(db_drawing *dp, short a)
	{
	cmljust=a;
	return RTNORM;
	}

sds_real db_header::RetCmlscale(db_drawing *dp)
	{
	return cmlscale;
	}

short db_header::SetCmlscale(db_drawing *dp, sds_real a)
	{
	cmlscale=a;
	return RTNORM;
	}

db_handitem *db_header::RetCmlstyle(db_drawing *dp)
	{
	return cmlstyle;
	}

short db_header::SetCmlstyle(db_drawing *dp, db_handitem *a)
	{
	cmlstyle=a;
	return RTNORM;
	}

short db_header::RetCoords(db_drawing *dp)
	{
	return coords;
	}

short db_header::SetCoords(db_drawing *dp, short a)
	{
	coords=a;
	return RTNORM;
	}

short db_header::RetCvport(db_drawing *dp)
	{
	return cvport;
	}

short db_header::SetCvport(db_drawing *dp, short a)
	{
	cvport=a;
	return RTNORM;
	}

short db_header::RetCyclecurr(db_drawing *dp)
	{
	return cyclecurr;
	}

short db_header::SetCyclecurr(db_drawing *dp, short a)
	{
	cyclecurr=a;
	return RTNORM;
	}

short db_header::RetCyclekeep(db_drawing *dp)
	{
	return cyclekeep;
	}

short db_header::SetCyclekeep(db_drawing *dp, short a)
	{
	cyclekeep=a;
	return RTNORM;
	}

short db_header::RetDbmod(db_drawing *dp)
	{
	return dbmod;
	}

short db_header::SetDbmod(db_drawing *dp, short a)
	{
	dbmod=a;
	return RTNORM;
	}

short db_header::RetDelobj(db_drawing *dp)
	{
	return delobj;
	}

short db_header::SetDelobj(db_drawing *dp, short a)
	{
	delobj=a;
	return RTNORM;
	}

short db_header::RetDiastat(db_drawing *dp)
	{
	return diastat;
	}

short db_header::SetDiastat(db_drawing *dp, short a)
	{
	diastat=a;
	return RTNORM;
	}

short db_header::RetDimalt(db_drawing *dp)
	{
	return dimalt;
	}

short db_header::SetDimalt(db_drawing *dp, short a)
	{
	dimalt=a;
	return RTNORM;
	}

short db_header::RetDimaltd(db_drawing *dp)
	{
	return dimaltd;
	}

short db_header::SetDimaltd(db_drawing *dp, short a)
	{
	dimaltd=a;
	return RTNORM;
	}

sds_real db_header::RetDimaltf(db_drawing *dp)
	{
	return dimaltf;
	}

short db_header::SetDimaltf(db_drawing *dp, sds_real a)
	{
	dimaltf=a;
	return RTNORM;
	}

short db_header::RetDimalttd(db_drawing *dp)
	{
	return dimalttd;
	}

short db_header::SetDimalttd(db_drawing *dp, short a)
	{
	dimalttd=a;
	return RTNORM;
	}

short db_header::RetDimalttz(db_drawing *dp)
	{
	return dimalttz;
	}

short db_header::SetDimalttz(db_drawing *dp, short a)
	{
	dimalttz=a;
	return RTNORM;
	}

short db_header::RetDimaltu(db_drawing *dp)
	{
	return dimaltu;
	}

short db_header::SetDimaltu(db_drawing *dp, short a)
	{
	dimaltu=a;
	return RTNORM;
	}

short db_header::RetDimaltz(db_drawing *dp)
	{
	return dimaltz;
	}

short db_header::SetDimaltz(db_drawing *dp, short a)
	{
	dimaltz=a;
	return RTNORM;
	}

char * db_header::RetDimapost(db_drawing *dp)
	{
	return dimapost;
	}

short db_header::SetDimapost(db_drawing *dp, char *a)
	{
	if (dimapost)
		delete dimapost;
	db_astrncpy(&dimapost,a,-1);
	return RTNORM;
	}

short db_header::RetDimaso(db_drawing *dp)
	{
	return dimaso;
	}

short db_header::SetDimaso(db_drawing *dp, short a)
	{
	dimaso=a;
	return RTNORM;
	}

sds_real db_header::RetDimasz(db_drawing *dp)
	{
	return dimasz;
	}

short db_header::SetDimasz(db_drawing *dp, sds_real a)
	{
	dimasz=a;
	return RTNORM;
	}

short db_header::RetDimaunit(db_drawing *dp)
	{
	return dimaunit;
	}

short db_header::SetDimaunit(db_drawing *dp, short a)
	{
	dimaunit=a;
	return RTNORM;
	}

char * db_header::RetDimblk(db_drawing *dp)
	{
	return dimblk;
	}

short db_header::SetDimblk(db_drawing *dp, char *a)
	{
	if (dimblk)
		delete dimblk;
	db_astrncpy(&dimblk,a,-1);
	return RTNORM;
	}

char * db_header::RetDimblk1(db_drawing *dp)
	{
	return dimblk1;
	}

short db_header::SetDimblk1(db_drawing *dp, char *a)
	{
	if (dimblk1)
		delete dimblk1;
	db_astrncpy(&dimblk1,a,-1);
	return RTNORM;
	}

char * db_header::RetDimblk2(db_drawing *dp)
	{
	return dimblk2;
	}

short db_header::SetDimblk2(db_drawing *dp, char *a)
	{
	if (dimblk2)
		delete dimblk2;
	db_astrncpy(&dimblk2,a,-1);
	return RTNORM;
	}

// EBATECH(watanabe)-[dimldrblk
char * db_header::RetDimldrblk(db_drawing *dp)
	{
	return dimldrblk;
	}

short db_header::SetDimldrblk(db_drawing *dp, char *a)
	{
	if (dimldrblk)
		delete dimldrblk;
	db_astrncpy(&dimldrblk,a,-1);
	return RTNORM;
	}
// ]-EBATECH(watanabe)

sds_real db_header::RetDimcen(db_drawing *dp)
	{
	return dimcen;
	}

short db_header::SetDimcen(db_drawing *dp, sds_real a)
	{
	dimcen=a;
	return RTNORM;
	}

short db_header::RetDimclrd(db_drawing *dp)
	{
	return dimclrd;
	}

short db_header::SetDimclrd(db_drawing *dp, short a)
	{
	dimclrd=a;
	return RTNORM;
	}

short db_header::RetDimclre(db_drawing *dp)
	{
	return dimclre;
	}

short db_header::SetDimclre(db_drawing *dp, short a)
	{
	dimclre=a;
	return RTNORM;
	}

short db_header::RetDimclrt(db_drawing *dp)
	{
	return dimclrt;
	}

short db_header::SetDimclrt(db_drawing *dp, short a)
	{
	dimclrt=a;
	return RTNORM;
	}
short db_header::RetDimadec(db_drawing *dp)
	{
	return dimadec;
	}

short db_header::SetDimadec(db_drawing *dp, short a)
	{
	dimadec=a;
	return RTNORM;
	}

short db_header::RetDimdec(db_drawing *dp)
	{
	return dimdec;
	}

short db_header::SetDimdec(db_drawing *dp, short a)
	{
	dimdec=a;
	return RTNORM;
	}

sds_real db_header::RetDimdle(db_drawing *dp)
	{
	return dimdle;
	}

short db_header::SetDimdle(db_drawing *dp, sds_real a)
	{
	dimdle=a;
	return RTNORM;
	}

sds_real db_header::RetDimdli(db_drawing *dp)
	{
	return dimdli;
	}

short db_header::SetDimdli(db_drawing *dp, sds_real a)
	{
	dimdli=a;
	return RTNORM;
	}

sds_real db_header::RetDimexe(db_drawing *dp)
	{
	return dimexe;
	}

short db_header::SetDimexe(db_drawing *dp, sds_real a)
	{
	dimexe=a;
	return RTNORM;
	}

sds_real db_header::RetDimexo(db_drawing *dp)
	{
	return dimexo;
	}

short db_header::SetDimexo(db_drawing *dp, sds_real a)
	{
	dimexo=a;
	return RTNORM;
	}

short db_header::RetDimfit(db_drawing *dp)
	{
	return dimfit;
	}

short db_header::SetDimfit(db_drawing *dp, short a)
	{
	dimfit=a;
	return RTNORM;
	}

sds_real db_header::RetDimgap(db_drawing *dp)
	{
	return dimgap;
	}

short db_header::SetDimgap(db_drawing *dp, sds_real a)
	{
	dimgap=a;
	return RTNORM;
	}

short db_header::RetDimjust(db_drawing *dp)
	{
	return dimjust;
	}

short db_header::SetDimjust(db_drawing *dp, short a)
	{
	dimjust=a;
	return RTNORM;
	}

sds_real db_header::RetDimlfac(db_drawing *dp)
	{
	return dimlfac;
	}

short db_header::SetDimlfac(db_drawing *dp, sds_real a)
	{
	dimlfac=a;
	return RTNORM;
	}

short db_header::RetDimlim(db_drawing *dp)
	{
	return dimlim;
	}

short db_header::SetDimlim(db_drawing *dp, short a)
	{
	dimlim=a;
	return RTNORM;
	}

char * db_header::RetDimpost(db_drawing *dp)
	{
	return dimpost;
	}

short db_header::SetDimpost(db_drawing *dp, char *a)
	{
	if (dimpost)
		delete dimpost;
	db_astrncpy(&dimpost,a,-1);
	return RTNORM;
	}

sds_real db_header::RetDimrnd(db_drawing *dp)
	{
	return dimrnd;
	}

short db_header::SetDimrnd(db_drawing *dp, sds_real a)
	{
	dimrnd=a;
	return RTNORM;
	}

short db_header::RetDimsah(db_drawing *dp)
	{
	return dimsah;
	}

short db_header::SetDimsah(db_drawing *dp, short a)
	{
	dimsah=a;
	return RTNORM;
	}

sds_real db_header::RetDimscale(db_drawing *dp)
	{
	return dimscale;
	}

short db_header::SetDimscale(db_drawing *dp, sds_real a)
	{
	dimscale=a;
	return RTNORM;
	}

short db_header::RetDimsd1(db_drawing *dp)
	{
	return dimsd1;
	}

short db_header::SetDimsd1(db_drawing *dp, short a)
	{
	dimsd1=a;
	return RTNORM;
	}

short db_header::RetDimsd2(db_drawing *dp)
	{
	return dimsd2;
	}

short db_header::SetDimsd2(db_drawing *dp, short a)
	{
	dimsd2=a;
	return RTNORM;
	}

short db_header::RetDimse1(db_drawing *dp)
	{
	return dimse1;
	}

short db_header::SetDimse1(db_drawing *dp, short a)
	{
	dimse1=a;
	return RTNORM;
	}

short db_header::RetDimse2(db_drawing *dp)
	{
	return dimse2;
	}

short db_header::SetDimse2(db_drawing *dp, short a)
	{
	dimse2=a;
	return RTNORM;
	}

short db_header::RetDimsho(db_drawing *dp)
	{
	return dimsho;
	}

short db_header::SetDimsho(db_drawing *dp, short a)
	{
	dimsho=a;
	return RTNORM;
	}

short db_header::RetDimsoxd(db_drawing *dp)
	{
	return dimsoxd;
	}

short db_header::SetDimsoxd(db_drawing *dp, short a)
	{
	dimsoxd=a;
	return RTNORM;
	}

db_handitem *db_header::RetDimstyle(db_drawing *dp)
	{
	return dimstyle;
	}

short db_header::SetDimstyle(db_drawing *dp, db_handitem *a)
	{
	dimstyle=a;
	return RTNORM;
	}

short db_header::RetDimtad(db_drawing *dp)
	{
	return dimtad;
	}

short db_header::SetDimtad(db_drawing *dp, short a)
	{
	dimtad=a;
	return RTNORM;
	}

short db_header::RetDimtdec(db_drawing *dp)
	{
	return dimtdec;
	}

short db_header::SetDimtdec(db_drawing *dp, short a)
	{
	dimtdec=a;
	return RTNORM;
	}

sds_real db_header::RetDimtfac(db_drawing *dp)
	{
	return dimtfac;
	}

short db_header::SetDimtfac(db_drawing *dp, sds_real a)
	{
	dimtfac=a;
	return RTNORM;
	}

short db_header::RetDimtih(db_drawing *dp)
	{
	return dimtih;
	}

short db_header::SetDimtih(db_drawing *dp, short a)
	{
	dimtih=a;
	return RTNORM;
	}

short db_header::RetDimtix(db_drawing *dp)
	{
	return dimtix;
	}

short db_header::SetDimtix(db_drawing *dp, short a)
	{
	dimtix=a;
	return RTNORM;
	}

sds_real db_header::RetDimtm(db_drawing *dp)
	{
	return dimtm;
	}

short db_header::SetDimtm(db_drawing *dp, sds_real a)
	{
	dimtm=a;
	return RTNORM;
	}

short db_header::RetDimtofl(db_drawing *dp)
	{
	return dimtofl;
	}

short db_header::SetDimtofl(db_drawing *dp, short a)
	{
	dimtofl=a;
	return RTNORM;
	}

short db_header::RetDimtoh(db_drawing *dp)
	{
	return dimtoh;
	}

short db_header::SetDimtoh(db_drawing *dp, short a)
	{
	dimtoh=a;
	return RTNORM;
	}

short db_header::RetDimtol(db_drawing *dp)
	{
	return dimtol;
	}

short db_header::SetDimtol(db_drawing *dp, short a)
	{
	dimtol=a;
	return RTNORM;
	}

short db_header::RetDimtolj(db_drawing *dp)
	{
	return dimtolj;
	}

short db_header::SetDimtolj(db_drawing *dp, short a)
	{
	dimtolj=a;
	return RTNORM;
	}


sds_real db_header::RetDimtp(db_drawing *dp)
	{
	return dimtp;
	}

short db_header::SetDimtp(db_drawing *dp, sds_real a)
	{
	dimtp=a;
	return RTNORM;
	}

sds_real db_header::RetDimtsz(db_drawing *dp)
	{
	return dimtsz;
	}

short db_header::SetDimtsz(db_drawing *dp, sds_real a)
	{
	dimtsz=a;
	return RTNORM;
	}

sds_real db_header::RetDimtvp(db_drawing *dp)
	{
	return dimtvp;
	}

short db_header::SetDimtvp(db_drawing *dp, sds_real a)
	{
	dimtvp=a;
	return RTNORM;
	}

db_handitem *db_header::RetDimtxsty(db_drawing *dp)
	{
	return dimtxsty;
	}

short db_header::SetDimtxsty(db_drawing *dp, db_handitem *a)
	{
	dimtxsty=a;
	return RTNORM;
	}

sds_real db_header::RetDimtxt(db_drawing *dp)
	{
	return dimtxt;
	}

short db_header::SetDimtxt(db_drawing *dp, sds_real a)
	{
	dimtxt=a;
	return RTNORM;
	}

short db_header::RetDimtzin(db_drawing *dp)
	{
	return dimtzin;
	}

short db_header::SetDimtzin(db_drawing *dp, short a)
	{
	dimtzin=a;
	return RTNORM;
	}

short db_header::RetDimunit(db_drawing *dp)
	{
	return dimunit;
	}

short db_header::SetDimunit(db_drawing *dp, short a)
	{
	dimunit=a;
	return RTNORM;
	}

short db_header::RetDimupt(db_drawing *dp)
	{
	return dimupt;
	}

short db_header::SetDimupt(db_drawing *dp, short a)
	{
	dimupt=a;
	return RTNORM;
	}

short db_header::RetDimzin(db_drawing *dp)
	{
	return dimzin;
	}

short db_header::SetDimzin(db_drawing *dp, short a)
	{
	dimzin=a;
	return RTNORM;
	}
// EBATECH(CNBR) -[ Bugzilla#78443 DIMAZIN 2003/2/6
short db_header::RetDimazin(db_drawing *dp)
	{
	return dimazin;
	}

short db_header::SetDimazin(db_drawing *dp, short a)
	{
	adimzin=a;
	return RTNORM;
	}
// EBATECH(CNBR) ]-
short db_header::RetDispsilh(db_drawing *dp)
	{
	return dispsilh;
	}

short db_header::SetDispsilh(db_drawing *dp, short a)
	{
	dispsilh=a;
	return RTNORM;
	}

sds_real db_header::RetDistance(db_drawing *dp)
	{
	return distance;
	}

short db_header::SetDistance(db_drawing *dp, sds_real a)
	{
	distance=a;
	return RTNORM;
	}

sds_real db_header::RetDonutid(db_drawing *dp)
	{
	return donutid;
	}

short db_header::SetDonutid(db_drawing *dp, sds_real a)
	{
	donutid=a;
	return RTNORM;
	}

sds_real db_header::RetDonutod(db_drawing *dp)
	{
	return donutod;
	}

short db_header::SetDonutod(db_drawing *dp, sds_real a)
	{
	donutod=a;
	return RTNORM;
	}

short db_header::RetDragmode(db_drawing *dp)
	{
	return dragmode;
	}

short db_header::SetDragmode(db_drawing *dp, short a)
	{
	dragmode=a;
	return RTNORM;
	}

short db_header::RetDwgcodepage(db_drawing *dp)
	{
	return dwgcodepage;
	}

short db_header::SetDwgcodepage(db_drawing *dp, short a)
	{
	dwgcodepage=a;
	return RTNORM;
	}

short db_header::RetDwgtitled(db_drawing *dp)
	{
	return dwgtitled;
	}

short db_header::SetDwgtitled(db_drawing *dp, short a)
	{
	dwgtitled=a;
	return RTNORM;
	}

short db_header::RetEdgemode(db_drawing *dp)
	{
	return edgemode;
	}

short db_header::SetEdgemode(db_drawing *dp, short a)
	{
	edgemode=a;
	return RTNORM;
	}

sds_real db_header::RetElevation(db_drawing *dp)
	{
    /* Trans WCS origin to UCS and add the Z to the value */
    /* in the buffer: */
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp!=NULL);
	if (dp==NULL)
		return 0.0;
	sds_point ap1;
    ap1[0]=ap1[1]=ap1[2]=0.0;
    dp->trans(ap1,&wcsrb,&ucsrb,0,ap1,NULL);
	return elevation+ap1[2];
	}

short db_header::SetElevation(db_drawing *dp, sds_real a)
	{
    /* Trans WCS origin to UCS and subtract the Z  */
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp!=NULL);
	if (dp==NULL)
		return RTERROR;
	sds_point ap1;
    ap1[0]=ap1[1]=ap1[2]=0.0;
    dp->trans(ap1,&wcsrb,&ucsrb,0,ap1,NULL);
	elevation=a-ap1[2];
	return RTNORM;
	}

short db_header::SetElevationAsIs(db_drawing *dp, sds_real a)
	{
	elevation=a;
	return RTNORM;
	}

short db_header::RetExplmode(db_drawing *dp)
	{
	return explmode;
	}

short db_header::SetExplmode(db_drawing *dp, short a)
	{
	explmode=a;
	return RTNORM;
	}

void db_header::GetExtmax(db_drawing *dp, sds_point a)
	{
	a[0]=extmax[0];
	a[1]=extmax[1];
	a[2]=extmax[2];
	}

short db_header::SetExtmax(db_drawing *dp, sds_point a)
	{
	extmax[0]=a[0];
	extmax[1]=a[1];
	extmax[2]=a[2];
	return RTNORM;
	}

void db_header::GetExtmin(db_drawing *dp, sds_point a)
	{
	a[0]=extmin[0];
	a[1]=extmin[1];
	a[2]=extmin[2];
	}

short db_header::SetExtmin(db_drawing *dp, sds_point a)
	{
	extmin[0]=a[0];
	extmin[1]=a[1];
	extmin[2]=a[2];
	return RTNORM;
	}

sds_real db_header::RetFacetres(db_drawing *dp)
	{
	return facetres;
	}

short db_header::SetFacetres(db_drawing *dp, sds_real a)
	{
	facetres=a;
	return RTNORM;
	}

short db_header::RetFastzoom(db_drawing *dp)
	{
	return fastzoom;
	}

short db_header::SetFastzoom(db_drawing *dp, short a)
	{
	fastzoom=a;
	return RTNORM;
	}

sds_real db_header::RetFilletrad(db_drawing *dp)
	{
	return filletrad;
	}

short db_header::SetFilletrad(db_drawing *dp, sds_real a)
	{
	filletrad=a;
	return RTNORM;
	}

short db_header::RetFillmode(db_drawing *dp)
	{
	return fillmode;
	}

short db_header::SetFillmode(db_drawing *dp, short a)
	{
	fillmode=a;
	return RTNORM;
	}

short db_header::RetFittype(db_drawing *dp)
	{
	return fittype;
	}

short db_header::SetFittype(db_drawing *dp, short a)
	{
	fittype=a;
	return RTNORM;
	}

short db_header::RetFlatland(db_drawing *dp)
	{
	return flatland;
	}

short db_header::SetFlatland(db_drawing *dp, short a)
	{
	flatland=a;
	return RTNORM;
	}

sds_real db_header::RetFrontz(db_drawing *dp)
	{
	return frontz;
	}

short db_header::SetFrontz(db_drawing *dp, sds_real a)
	{
	frontz=a;
	return RTNORM;
	}

void db_header::GetGpslastpt(db_drawing *dp, sds_point a)
	{
	a[0]=gpslastpt[0];
	a[1]=gpslastpt[1];
	a[2]=gpslastpt[2];
	}

short db_header::SetGpslastpt(db_drawing *dp, sds_point a)
	{
	gpslastpt[0]=a[0];
	gpslastpt[1]=a[1];
	gpslastpt[2]=a[2];
	return RTNORM;
	}

short db_header::RetGpsmode(db_drawing *dp)
	{
	return gpsmode;
	}

short db_header::SetGpsmode(db_drawing *dp, short a)
	{
	gpsmode=a;
	return RTNORM;
	}

short db_header::RetGpspaused(db_drawing *dp)
	{
	return gpspaused;
	}

short db_header::SetGpspaused(db_drawing *dp, short a)
	{
	gpspaused=a;
	return RTNORM;
	}

char * db_header::RetGpsplayed(db_drawing *dp)
	{
	return gpsplayed;
	}

short db_header::SetGpsplayed(db_drawing *dp, char *a)
	{
	if (gpsplayed)
		delete gpsplayed;
	db_astrncpy(&gpsplayed,a,-1);
	return RTNORM;
	}

short db_header::RetGridmode(db_drawing *dp)
	{
	return gridmode;
	}

short db_header::SetGridmode(db_drawing *dp, short a)
	{
	gridmode=a;
	return RTNORM;
	}

void db_header::GetGridunit(db_drawing *dp, sds_point a)
	{
	a[0]=gridunit[0];
	a[1]=gridunit[1];
	a[2]=gridunit[2];
	}

short db_header::SetGridunit(db_drawing *dp, sds_point a)
	{
	if (a[0]<0.0 || a[1]<0.0)
		return RTERROR;
	gridunit[0]=a[0];
	gridunit[1]=a[1];
	gridunit[2]=a[2];
	return RTNORM;
	}

short db_header::RetHandles(db_drawing *dp)
	{
	return handles;
	}

short db_header::SetHandles(db_drawing *dp, short a)
	{
	handles=a;
	return RTNORM;
	}

db_objhandle	db_header::RetHandseed(db_drawing *dp)
	{
	return handseed;
	}

short db_header::SetHandseed(db_drawing *dp, db_objhandle a)
	{
	handseed=a;
	return RTNORM;
	}

short db_header::RetHighlight(db_drawing *dp)
	{
	return highlight;
	}

short db_header::SetHighlight(db_drawing *dp, short a)
	{
	highlight=a;
	return RTNORM;
	}

sds_real db_header::RetHpang(db_drawing *dp)
	{
	return hpang;
	}

short db_header::SetHpang(db_drawing *dp, sds_real a)
	{
	hpang=a;
	return RTNORM;
	}

short db_header::RetHpbound(db_drawing *dp)
	{
	return hpbound;
	}

short db_header::SetHpbound(db_drawing *dp, short a)
	{
	hpbound=a;
	return RTNORM;
	}

short db_header::RetHpdouble(db_drawing *dp)
	{
	return hpdouble;
	}

short db_header::SetHpdouble(db_drawing *dp, short a)
	{
	hpdouble=a;
	return RTNORM;
	}

char * db_header::RetHpname(db_drawing *dp)
	{
	return hpname;
	}

short db_header::SetHpname(db_drawing *dp, char *a)
	{
	if (hpname)
		delete hpname;
	db_astrncpy(&hpname,a,-1);
	return RTNORM;
	}

sds_real db_header::RetHpscale(db_drawing *dp)
	{
	return hpscale;
	}

short db_header::SetHpscale(db_drawing *dp, sds_real a)
	{
	hpscale=a;
	return RTNORM;
	}

sds_real db_header::RetHpspace(db_drawing *dp)
	{
	return hpspace;
	}

short db_header::SetHpspace(db_drawing *dp, sds_real a)
	{
	hpspace=a;
	return RTNORM;
	}

short db_header::RetHpstyle(db_drawing *dp)
	{
	return hpstyle;
	}

short db_header::SetHpstyle(db_drawing *dp, short a)
	{
	hpstyle=a;
	return RTNORM;
	}

void db_header::GetInsbase(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return;
	dp->trans(insbase,&wcsrb,&ucsrb,0,a,NULL);
	}

short db_header::SetInsbase(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return RTERROR;
	dp->trans(a,&ucsrb,&wcsrb,0,insbase,NULL);
	return RTNORM;
	}

short db_header::SetInsbaseAsIs(db_drawing *dp, sds_point a)
	{
	insbase[0]=a[0];
	insbase[1]=a[1];
	insbase[2]=a[2];
	return RTNORM;
	}

char * db_header::RetInsname(db_drawing *dp)
	{
	return insname;
	}

short db_header::SetInsname(db_drawing *dp, char *a)
	{
	if (insname)
		delete insname;
	db_astrncpy(&insname,a,-1);
	return RTNORM;
	}

short db_header::RetIslastcmdpt(db_drawing *dp)
	{
	return islastcmdpt;
	}

short db_header::SetIslastcmdpt(db_drawing *dp, short a)
	{
	islastcmdpt=a;
	return RTNORM;
	}

short db_header::RetIsolines(db_drawing *dp)
	{
	return isolines;
	}

short db_header::SetIsolines(db_drawing *dp, short a)
	{
	isolines=a;
	return RTNORM;
	}

sds_real db_header::RetLastangle(db_drawing *dp)
	{
	return lastangle;
	}

short db_header::SetLastangle(db_drawing *dp, sds_real a)
	{
	lastangle=a;
	return RTNORM;
	}

sds_real db_header::RetLastcmdang(db_drawing *dp)
	{
	return lastcmdang;
	}

short db_header::SetLastcmdang(db_drawing *dp, sds_real a)
	{
	lastcmdang=a;
	return RTNORM;
	}

void db_header::GetLastcmdpt(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return;
	dp->trans(lastcmdpt,&wcsrb,&ucsrb,0,a,NULL);
	}

short db_header::SetLastcmdpt(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return RTERROR;
	dp->trans(a,&ucsrb,&wcsrb,0,lastcmdpt,NULL);
	return RTNORM;
	}

void db_header::GetLastpoint(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return;
	dp->trans(lastpoint,&wcsrb,&ucsrb,0,a,NULL);
	}

short db_header::SetLastpoint(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return RTERROR;
	dp->trans(a,&ucsrb,&wcsrb,0,lastpoint,NULL);
	return RTNORM;
	}

sds_real db_header::RetLenslength(db_drawing *dp)
	{
	return lenslength;
	}

short db_header::SetLenslength(db_drawing *dp, sds_real a)
	{
	lenslength=a;
	return RTNORM;
	}

short db_header::RetLimcheck(db_drawing *dp)
	{
	return limcheck;
	}

short db_header::SetLimcheck(db_drawing *dp, short a)
	{
	limcheck=a;
	return RTNORM;
	}

void db_header::GetLimmax(db_drawing *dp, sds_point a)
	{
	a[0]=limmax[0];
	a[1]=limmax[1];
	a[2]=limmax[2];
	}

short db_header::SetLimmax(db_drawing *dp, sds_point a)
	{
	limmax[0]=a[0];
	limmax[1]=a[1];
	limmax[2]=a[2];
	return RTNORM;
	}

void db_header::GetLimmin(db_drawing *dp, sds_point a)
	{
	a[0]=limmin[0];
	a[1]=limmin[1];
	a[2]=limmin[2];
	}

short db_header::SetLimmin(db_drawing *dp, sds_point a)
	{
	limmin[0]=a[0];
	limmin[1]=a[1];
	limmin[2]=a[2];
	return RTNORM;
	}

sds_real db_header::RetLtscale(db_drawing *dp)
	{
	return ltscale;
	}

short db_header::SetLtscale(db_drawing *dp, sds_real a)
	{
	ltscale=a;
	return RTNORM;
	}

short db_header::RetLtsclwblk(db_drawing *dp)
	{
	return ltsclwblk;
	}

short db_header::SetLtsclwblk(db_drawing *dp, short a)
	{
	ltsclwblk=a;
	return RTNORM;
	}

short db_header::RetLunits(db_drawing *dp)
	{
	return lunits;
	}

short db_header::SetLunits(db_drawing *dp, short a)
	{
	lunits=a;
	return RTNORM;
	}

short db_header::RetLuprec(db_drawing *dp)
	{
	return luprec;
	}

short db_header::SetLuprec(db_drawing *dp, short a)
	{
	luprec=a;
	return RTNORM;
	}

short db_header::RetMaxactvp(db_drawing *dp)
	{
	return maxactvp;
	}

short db_header::SetMaxactvp(db_drawing *dp, short a)
	{
	maxactvp=a;
	return RTNORM;
	}

short db_header::RetMeasurement(db_drawing *dp)
	{
	return measurement;
	}

short db_header::SetMeasurement(db_drawing *dp, short a)
	{
	measurement=a;
	return RTNORM;
	}

short db_header::RetMenuecho(db_drawing *dp)
	{
	return menuecho;
	}

short db_header::SetMenuecho(db_drawing *dp, short a)
	{
	menuecho=a;
	return RTNORM;
	}

char * db_header::RetMenuname(db_drawing *dp)
	{
	return menuname;
	}

short db_header::SetMenuname(db_drawing *dp, char *a)
	{
	char menubuf[1024];
	strcpy(menubuf,a);
	ic_trim(menubuf,"be"/*DNT*/);
// original code had check for max menu size of 58 here . . . seems too limiting
	if (menuname)
		delete menuname;
	db_astrncpy(&menuname,menubuf,-1);
	return RTNORM;
	}

short db_header::RetMirrtext(db_drawing *dp)
	{
	return mirrtext;
	}

short db_header::SetMirrtext(db_drawing *dp, short a)
	{
	mirrtext=a;
	return RTNORM;
	}

sds_real db_header::RetOffsetdist(db_drawing *dp)
	{
	return offsetdist;
	}

short db_header::SetOffsetdist(db_drawing *dp, sds_real a)
	{
	offsetdist=a;
	return RTNORM;
	}

short db_header::RetOrthomode(db_drawing *dp)
	{
	return orthomode;
	}

short db_header::SetOrthomode(db_drawing *dp, short a)
	{
	orthomode=a;
	return RTNORM;
	}

short db_header::RetOsmode(db_drawing *dp)
	{
	return osmode;
	}

short db_header::SetOsmode(db_drawing *dp, short a)
	{
	osmode=a;
	return RTNORM;
	}

short db_header::RetPdmode(db_drawing *dp)
	{
	return pdmode;
	}

short db_header::SetPdmode(db_drawing *dp, short a)
	{
	pdmode=a;
	return RTNORM;
	}

sds_real db_header::RetPdsize(db_drawing *dp)
	{
	return pdsize;
	}

short db_header::SetPdsize(db_drawing *dp, sds_real a)
	{
	pdsize=a;
	return RTNORM;
	}

short db_header::RetPellipse(db_drawing *dp)
	{
	return pellipse;
	}

short db_header::SetPellipse(db_drawing *dp, short a)
	{
	pellipse=a;
	return RTNORM;
	}

sds_real db_header::RetPerimeter(db_drawing *dp)
	{
	return perimeter;
	}

short db_header::SetPerimeter(db_drawing *dp, sds_real a)
	{
	perimeter=a;
	return RTNORM;
	}

short db_header::RetPickstyle(db_drawing *dp)
	{
	return pickstyle;
	}

short db_header::SetPickstyle(db_drawing *dp, short a)
	{
	pickstyle=a;
	return RTNORM;
	}

short db_header::RetPlinegen(db_drawing *dp)
	{
	return plinegen;
	}

short db_header::SetPlinegen(db_drawing *dp, short a)
	{
	plinegen=a;
	return RTNORM;
	}

sds_real db_header::RetPlinewid(db_drawing *dp)
	{
	return plinewid;
	}

short db_header::SetPlinewid(db_drawing *dp, sds_real a)
	{
	plinewid=a;
	return RTNORM;
	}

short db_header::RetPolysides(db_drawing *dp)
	{
	return polysides;
	}

short db_header::SetPolysides(db_drawing *dp, short a)
	{
	polysides=a;
	return RTNORM;
	}

short db_header::RetProxygraphics(db_drawing *dp)
	{
	return proxygraphics;
	}

short db_header::SetProxygraphics(db_drawing *dp, short a)
	{
	proxygraphics=a;
	return RTNORM;
	}

short db_header::RetPsltscale(db_drawing *dp)
	{
	return psltscale;
	}

short db_header::SetPsltscale(db_drawing *dp, short a)
	{
	psltscale=a;
	return RTNORM;
	}

sds_real db_header::RetP_elevation(db_drawing *dp)
	{
    /* Trans WCS origin to UCS and add the Z to the value */
    /* in the buffer: */
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp!=NULL);
	if (dp==NULL)
		return 0.0;
	sds_point ap1;
    ap1[0]=ap1[1]=ap1[2]=0.0;
    dp->trans(ap1,&wcsrb,&ucsrb,0,ap1,NULL);
	return p_elevation+ap1[2];
	}

short db_header::SetP_elevation(db_drawing *dp, sds_real a)
	{
	p_elevation=a;
	return RTNORM;
	}

short db_header::SetP_elevationAsIs(db_drawing *dp, sds_real a)
	{
	p_elevation=a;
	return RTNORM;
	}

void db_header::GetP_extmax(db_drawing *dp, sds_point a)
	{
	a[0]=p_extmax[0];
	a[1]=p_extmax[1];
	a[2]=p_extmax[2];
	}

short db_header::SetP_extmax(db_drawing *dp, sds_point a)
	{
	p_extmax[0]=a[0];
	p_extmax[1]=a[1];
	p_extmax[2]=a[2];
	return RTNORM;
	}

void db_header::GetP_extmin(db_drawing *dp, sds_point a)
	{
	a[0]=p_extmin[0];
	a[1]=p_extmin[1];
	a[2]=p_extmin[2];
	}

short db_header::SetP_extmin(db_drawing *dp, sds_point a)
	{
	p_extmin[0]=a[0];
	p_extmin[1]=a[1];
	p_extmin[2]=a[2];
	return RTNORM;
	}

void db_header::GetP_insbase(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return;
	dp->trans(p_insbase,&wcsrb,&ucsrb,0,a,NULL);
	}

short db_header::SetP_insbase(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return RTNORM;
	dp->trans(a,&ucsrb,&wcsrb,0,p_insbase,NULL);
	return RTNORM;
	}

short db_header::SetP_insbaseAsIs(db_drawing *dp, sds_point a)
	{
	p_insbase[0]=a[0];
	p_insbase[1]=a[1];
	p_insbase[2]=a[2];
	return RTNORM;
	}

short db_header::RetP_limcheck(db_drawing *dp)
	{
	return p_limcheck;
	}

short db_header::SetP_limcheck(db_drawing *dp, short a)
	{
	p_limcheck=a;
	return RTNORM;
	}

void db_header::GetP_limmax(db_drawing *dp, sds_point a)
	{
	a[0]=p_limmax[0];
	a[1]=p_limmax[1];
	a[2]=p_limmax[2];
	}

short db_header::SetP_limmax(db_drawing *dp, sds_point a)
	{
	p_limmax[0]=a[0];
	p_limmax[1]=a[1];
	p_limmax[2]=a[2];
	return RTNORM;
	}

void db_header::GetP_limmin(db_drawing *dp, sds_point a)
	{
	a[0]=p_limmin[0];
	a[1]=p_limmin[1];
	a[2]=p_limmin[2];
	}

short db_header::SetP_limmin(db_drawing *dp, sds_point a)
	{
	p_limmin[0]=a[0];
	p_limmin[1]=a[1];
	p_limmin[2]=a[2];
	return RTNORM;
	}

db_handitem *db_header::RetP_ucsname(db_drawing *dp)
	{
	return p_ucsname;
	}

short db_header::SetP_ucsname(db_drawing *dp, db_handitem *a)
	{
	p_ucsname=a;
	return RTNORM;
	}

void db_header::GetP_ucsorg(db_drawing *dp, sds_point a)
	{
	a[0]=p_ucsorg[0];
	a[1]=p_ucsorg[1];
	a[2]=p_ucsorg[2];
	}

short db_header::SetP_ucsorg(db_drawing *dp, sds_point a)
	{
	p_ucsorg[0]=a[0];
	p_ucsorg[1]=a[1];
	p_ucsorg[2]=a[2];
	return RTNORM;
	}

void db_header::GetP_ucsxdir(db_drawing *dp, sds_point a)
	{
	a[0]=p_ucsxdir[0];
	a[1]=p_ucsxdir[1];
	a[2]=p_ucsxdir[2];
	}

short db_header::SetP_ucsxdir(db_drawing *dp, sds_point a)
	{
	if (icadRealEqual(a[0],0.0) && icadRealEqual(a[1],0.0) && icadRealEqual(a[2],0.0))
		return RTERROR;
	p_ucsxdir[0]=a[0];
	p_ucsxdir[1]=a[1];
	p_ucsxdir[2]=a[2];
	return RTNORM;
	}

void db_header::GetP_ucsydir(db_drawing *dp, sds_point a)
	{
	a[0]=p_ucsydir[0];
	a[1]=p_ucsydir[1];
	a[2]=p_ucsydir[2];
	}

short db_header::SetP_ucsydir(db_drawing *dp, sds_point a)
	{
	if (icadRealEqual(a[0],0.0) && icadRealEqual(a[1],0.0) && icadRealEqual(a[2],0.0))
		return RTERROR;
	p_ucsydir[0]=a[0];
	p_ucsydir[1]=a[1];
	p_ucsydir[2]=a[2];
	return RTNORM;
	}

short db_header::RetQtextmode(db_drawing *dp)
	{
	return qtextmode;
	}

short db_header::SetQtextmode(db_drawing *dp, short a)
	{
	qtextmode=a;
	return RTNORM;
	}

short db_header::RetRegenmode(db_drawing *dp)
	{
	return regenmode;
	}

short db_header::SetRegenmode(db_drawing *dp, short a)
	{
	regenmode=a;
	return RTNORM;
	}

short db_header::RetSaveimages(db_drawing *dp)
	{
	return saveimages;
	}

short db_header::SetSaveimages(db_drawing *dp, short a)
	{
	saveimages=a;
	return RTNORM;
	}

char * db_header::RetSavename(db_drawing *dp)
	{
	return savename;
	}

short db_header::SetSavename(db_drawing *dp, char *a)
	{
	if (savename)
		delete savename;
	db_astrncpy(&savename,a,-1);
	return RTNORM;
	}

short db_header::RetShadedge(db_drawing *dp)
	{
	return shadedge;
	}

short db_header::SetShadedge(db_drawing *dp, short a)
	{
	shadedge=a;
	return RTNORM;
	}

short db_header::RetShadedif(db_drawing *dp)
	{
	return shadedif;
	}

short db_header::SetShadedif(db_drawing *dp, short a)
	{
	shadedif=a;
	return RTNORM;
	}

char * db_header::RetShpname(db_drawing *dp)
	{
	return shpname;
	}

short db_header::SetShpname(db_drawing *dp, char *a)
	{
	if (shpname)
		delete shpname;
	db_astrncpy(&shpname,a,-1);
	return RTNORM;
	}

sds_real db_header::RetSketchinc(db_drawing *dp)
	{
	return sketchinc;
	}

short db_header::SetSketchinc(db_drawing *dp, sds_real a)
	{
	sketchinc=a;
	return RTNORM;
	}

short db_header::RetSkpoly(db_drawing *dp)
	{
	return skpoly;
	}

short db_header::SetSkpoly(db_drawing *dp, short a)
	{
	skpoly=a;
	return RTNORM;
	}

sds_real db_header::RetSnapang(db_drawing *dp)
	{
	return snapang;
	}

short db_header::SetSnapang(db_drawing *dp, sds_real a)
	{
	snapang=a;
	return RTNORM;
	}

void db_header::GetSnapbase(db_drawing *dp, sds_point a)
	{
	a[0]=snapbase[0];
	a[1]=snapbase[1];
	a[2]=snapbase[2];
	}

short db_header::SetSnapbase(db_drawing *dp, sds_point a)
	{
	snapbase[0]=a[0];
	snapbase[1]=a[1];
	snapbase[2]=a[2];
	return RTNORM;
	}

short db_header::RetSnapisopair(db_drawing *dp)
	{
	return snapisopair;
	}

short db_header::SetSnapisopair(db_drawing *dp, short a)
	{
	snapisopair=a;
	return RTNORM;
	}

short db_header::RetSnapmode(db_drawing *dp)
	{
	return snapmode;
	}

short db_header::SetSnapmode(db_drawing *dp, short a)
	{
	snapmode=a;
	return RTNORM;
	}

short db_header::RetSnapstyl(db_drawing *dp)
	{
	return snapstyl;
	}

short db_header::SetSnapstyl(db_drawing *dp, short a)
	{
	snapstyl=a;
	return RTNORM;
	}

void db_header::GetSnapunit(db_drawing *dp, sds_point a)
	{
	a[0]=snapunit[0];
	a[1]=snapunit[1];
	a[2]=snapunit[2];
	}

short db_header::SetSnapunit(db_drawing *dp, sds_point a)
	{
	if (a[0]<=0.0 || a[1]<=0.0)
		return RTERROR;
	snapunit[0]=a[0];
	snapunit[1]=a[1];
	snapunit[2]=a[2];
	return RTNORM;
	}

short db_header::RetSplframe(db_drawing *dp)
	{
	return splframe;
	}

short db_header::SetSplframe(db_drawing *dp, short a)
	{
	splframe=a;
	return RTNORM;
	}

short db_header::RetSplinesegs(db_drawing *dp)
	{
	return splinesegs;
	}

short db_header::SetSplinesegs(db_drawing *dp, short a)
	{
	splinesegs=a;
	return RTNORM;
	}

short db_header::RetSplinetype(db_drawing *dp)
	{
	return splinetype;
	}

short db_header::SetSplinetype(db_drawing *dp, short a)
	{
	splinetype=a;
	return RTNORM;
	}

short db_header::RetSurftab1(db_drawing *dp)
	{
	return surftab1;
	}

short db_header::SetSurftab1(db_drawing *dp, short a)
	{
	surftab1=a;
	return RTNORM;
	}

short db_header::RetSurftab2(db_drawing *dp)
	{
	return surftab2;
	}

short db_header::SetSurftab2(db_drawing *dp, short a)
	{
	surftab2=a;
	return RTNORM;
	}

short db_header::RetSurftype(db_drawing *dp)
	{
	return surftype;
	}

short db_header::SetSurftype(db_drawing *dp, short a)
	{
	if (a!=5 && a!=6 && a!=8)
		return RTERROR;
	surftype=a;
	return RTNORM;
	}

short db_header::RetSurfu(db_drawing *dp)
	{
	return surfu;
	}

short db_header::SetSurfu(db_drawing *dp, short a)
	{
	surfu=a;
	return RTNORM;
	}

short db_header::RetSurfv(db_drawing *dp)
	{
	return surfv;
	}

short db_header::SetSurfv(db_drawing *dp, short a)
	{
	surfv=a;
	return RTNORM;
	}

void db_header::GetTarget(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return;
	dp->trans(target,&wcsrb,&ucsrb,0,a,NULL);
	}

short db_header::SetTarget(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return RTERROR;
	dp->trans(a,&ucsrb,&wcsrb,0,target,NULL);
	return RTNORM;
	}

short db_header::SetTargetAsIs(db_drawing *dp, sds_point a)
	{
	target[0]=a[0];
	target[1]=a[1];
	target[2]=a[2];
	return RTNORM;
	}

sds_real db_header::RetTdcreate(db_drawing *dp)
	{
	return tdcreate;
	}

short db_header::SetTdcreate(db_drawing *dp, sds_real a)
	{
	tdcreate=a;
	return RTNORM;
	}

sds_real db_header::RetTdindwg(db_drawing *dp)
	{
	return (time(NULL)-dp->ret_sessionstarttime())/86400.0+tdindwg;
	}

short db_header::SetTdindwg(db_drawing *dp, sds_real a)
	{
	tdindwg=a;
	return RTNORM;
	}

sds_real db_header::RetTdupdate(db_drawing *dp)
	{
	return tdupdate;
	}

short db_header::SetTdupdate(db_drawing *dp, sds_real a)
	{
	tdupdate=a;
	return RTNORM;
	}

sds_real db_header::RetTdusrtimer(db_drawing *dp)
	{
	if (usrtimer)
		return (time(NULL)-dp->ret_usrstarttime())/86400.0+tdusrtimer;
	else
		return tdusrtimer;
	}

short db_header::SetTdusrtimer(db_drawing *dp, sds_real a)
	{
	tdusrtimer=a;
	dp->set_usrstarttime();
	return RTNORM;
	}

sds_real db_header::RetTextangle(db_drawing *dp)
	{
	return textangle;
	}

short db_header::SetTextangle(db_drawing *dp, sds_real a)
	{
	textangle=a;
	return RTNORM;
	}

short db_header::RetTextqlty(db_drawing *dp)
	{
	return textqlty;
	}

short db_header::SetTextqlty(db_drawing *dp, short a)
	{
	textqlty=a;
	return RTNORM;
	}

sds_real db_header::RetTextsize(db_drawing *dp)
	{
	return textsize;
	}

short db_header::SetTextsize(db_drawing *dp, sds_real a)
	{
	textsize=a;
	return RTNORM;
	}

db_handitem *db_header::RetTextstyle(db_drawing *dp)
	{
	return textstyle;
	}

short db_header::SetTextstyle(db_drawing *dp, db_handitem *a)
	{
	textstyle=a;
	return RTNORM;
	}

sds_real db_header::RetThickness(db_drawing *dp)
	{
	return thickness;
	}

short db_header::SetThickness(db_drawing *dp, sds_real a)
	{
	thickness=a;
	return RTNORM;
	}

short db_header::RetTilemode(db_drawing *dp)
	{
	return tilemode;
	}

short db_header::SetTilemode(db_drawing *dp, short a)
	{
	tilemode=a;
	return RTNORM;
	}

sds_real db_header::RetTracewid(db_drawing *dp)
	{
	return tracewid;
	}

short db_header::SetTracewid(db_drawing *dp, sds_real a)
	{
	tracewid=a;
	return RTNORM;
	}

short db_header::RetTreedepth(db_drawing *dp)
	{
	return treedepth;
	}

short db_header::SetTreedepth(db_drawing *dp, short a)
	{
	treedepth=a;
	return RTNORM;
	}

short db_header::RetTrimmode(db_drawing *dp)
	{
	return trimmode;
	}

short db_header::SetTrimmode(db_drawing *dp, short a)
	{
	trimmode=a;
	return RTNORM;
	}

short db_header::RetUcsfollow(db_drawing *dp)
	{
	return ((ucsfollow & 8) != 0);
	}

short db_header::SetUcsfollow(db_drawing *dp, short a)
	{
	if (a)
		ucsfollow |= 8;
	else
		ucsfollow &= ~8;
	return RTNORM;
	}

short db_header::RetUcsicon(db_drawing *dp)
	{
	return ucsicon;
	}

short db_header::SetUcsicon(db_drawing *dp, short a)
	{
	ucsicon=a;
	return RTNORM;
	}

db_handitem *db_header::RetUcsname(db_drawing *dp)
	{
	return ucsname;
	}

short db_header::SetUcsname(db_drawing *dp, db_handitem *a)
	{
	ucsname=a;
	return RTNORM;
	}

void db_header::GetUcsorg(db_drawing *dp, sds_point a)
	{
	a[0]=ucsorg[0];
	a[1]=ucsorg[1];
	a[2]=ucsorg[2];
	}

short db_header::SetUcsorg(db_drawing *dp, sds_point a)
	{
	ucsorg[0]=a[0];
	ucsorg[1]=a[1];
	ucsorg[2]=a[2];
	return RTNORM;
	}

void db_header::GetUcsxdir(db_drawing *dp, sds_point a)
	{
	a[0]=ucsxdir[0];
	a[1]=ucsxdir[1];
	a[2]=ucsxdir[2];
	}

short db_header::SetUcsxdir(db_drawing *dp, sds_point a)
	{
	if (icadRealEqual(a[0],0.0) && icadRealEqual(a[1],0.0) && icadRealEqual(a[2],0.0))
		return RTERROR;
	ucsxdir[0]=a[0];
	ucsxdir[1]=a[1];
	ucsxdir[2]=a[2];
	return RTNORM;
	}

void db_header::GetUcsydir(db_drawing *dp, sds_point a)
	{
	a[0]=ucsydir[0];
	a[1]=ucsydir[1];
	a[2]=ucsydir[2];
	}

short db_header::SetUcsydir(db_drawing *dp, sds_point a)
	{
	if (icadRealEqual(a[0],0.0) && icadRealEqual(a[1],0.0) && icadRealEqual(a[2],0.0))
		return RTERROR;
	ucsydir[0]=a[0];
	ucsydir[1]=a[1];
	ucsydir[2]=a[2];
	return RTNORM;
	}

short db_header::RetUndoctl(db_drawing *dp)
	{
	return undoctl;
	}

short db_header::SetUndoctl(db_drawing *dp, short a)
	{
	undoctl=a;
	return RTNORM;
	}

short db_header::RetUndomarks(db_drawing *dp)
	{
	return undomarks;
	}

short db_header::SetUndomarks(db_drawing *dp, short a)
	{
	undomarks=a;
	return RTNORM;
	}

short db_header::RetUnitmode(db_drawing *dp)
	{
	return unitmode;
	}

short db_header::SetUnitmode(db_drawing *dp, short a)
	{
	unitmode=a;
	return RTNORM;
	}

short db_header::RetUseri1(db_drawing *dp)
	{
	return useri1;
	}

short db_header::SetUseri1(db_drawing *dp, short a)
	{
	useri1=a;
	return RTNORM;
	}

short db_header::RetUseri2(db_drawing *dp)
	{
	return useri2;
	}

short db_header::SetUseri2(db_drawing *dp, short a)
	{
	useri2=a;
	return RTNORM;
	}

short db_header::RetUseri3(db_drawing *dp)
	{
	return useri3;
	}

short db_header::SetUseri3(db_drawing *dp, short a)
	{
	useri3=a;
	return RTNORM;
	}

short db_header::RetUseri4(db_drawing *dp)
	{
	return useri4;
	}

short db_header::SetUseri4(db_drawing *dp, short a)
	{
	useri4=a;
	return RTNORM;
	}

short db_header::RetUseri5(db_drawing *dp)
	{
	return useri5;
	}

short db_header::SetUseri5(db_drawing *dp, short a)
	{
	useri5=a;
	return RTNORM;
	}

sds_real db_header::RetUserr1(db_drawing *dp)
	{
	return userr1;
	}

short db_header::SetUserr1(db_drawing *dp, sds_real a)
	{
	userr1=a;
	return RTNORM;
	}

sds_real db_header::RetUserr2(db_drawing *dp)
	{
	return userr2;
	}

short db_header::SetUserr2(db_drawing *dp, sds_real a)
	{
	userr2=a;
	return RTNORM;
	}

sds_real db_header::RetUserr3(db_drawing *dp)
	{
	return userr3;
	}

short db_header::SetUserr3(db_drawing *dp, sds_real a)
	{
	userr3=a;
	return RTNORM;
	}

sds_real db_header::RetUserr4(db_drawing *dp)
	{
	return userr4;
	}

short db_header::SetUserr4(db_drawing *dp, sds_real a)
	{
	userr4=a;
	return RTNORM;
	}

sds_real db_header::RetUserr5(db_drawing *dp)
	{
	return userr5;
	}

short db_header::SetUserr5(db_drawing *dp, sds_real a)
	{
	userr5=a;
	return RTNORM;
	}

char * db_header::RetUsers1(db_drawing *dp)
	{
	return users1;
	}

short db_header::SetUsers1(db_drawing *dp, char *a)
	{
	if (users1)
		delete users1;
	db_astrncpy(&users1,a,-1);
	return RTNORM;
	}

char * db_header::RetUsers2(db_drawing *dp)
	{
	return users2;
	}

short db_header::SetUsers2(db_drawing *dp, char *a)
	{
	if (users2)
		delete users2;
	db_astrncpy(&users2,a,-1);
	return RTNORM;
	}

char * db_header::RetUsers3(db_drawing *dp)
	{
	return users3;
	}

short db_header::SetUsers3(db_drawing *dp, char *a)
	{
	if (users3)
		delete users3;
	db_astrncpy(&users3,a,-1);
	return RTNORM;
	}

char * db_header::RetUsers4(db_drawing *dp)
	{
	return users4;
	}

short db_header::SetUsers4(db_drawing *dp, char *a)
	{
	if (users4)
		delete users4;
	db_astrncpy(&users4,a,-1);
	return RTNORM;
	}

char * db_header::RetUsers5(db_drawing *dp)
	{
	return users5;
	}

short db_header::SetUsers5(db_drawing *dp, char *a)
	{
	if (users5)
		delete users5;
	db_astrncpy(&users5,a,-1);
	return RTNORM;
	}

short db_header::RetUsrtimer(db_drawing *dp)
	{
	return usrtimer;
	}

short db_header::SetUsrtimer(db_drawing *dp, short a)
	{
	ASSERT(dp);
	if (dp==NULL) 
		return RTERROR;
	// NOTE: act ONLY if we are changing the state of the variable
	if (usrtimer==0 && a!=0)  // toggling on
		{
		dp->set_usrstarttime();
		usrtimer=1;
		}
	else if (usrtimer!=0 && a==0) // toggling off
		{
		tdusrtimer=this->RetTdusrtimer(dp);	// yes, this is correct.  Sets the usrtimer field to
											// the final user time when this variable was turned off
		usrtimer=0;
		}
	return RTNORM;
	}

sds_real db_header::RetViewaspect(db_drawing *dp)
	{
	return viewaspect;
	}

short db_header::SetViewaspect(db_drawing *dp, sds_real a)
	{
	viewaspect=a;
	return RTNORM;
	}

void db_header::GetViewctr(db_drawing *dp, sds_point a)
	{
	struct resbuf dcsrb,ucsrb;
	dcsrb.restype=ucsrb.restype=RTSHORT;
	dcsrb.resval.rint=2;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return;
	dp->trans(viewctr,&dcsrb,&ucsrb,0,a,NULL);
	dp->movept2elev(a,a);
	}

short db_header::SetViewctr(db_drawing *dp, sds_point a)
	{
	struct resbuf dcsrb,ucsrb;
	dcsrb.restype=ucsrb.restype=RTSHORT;
	dcsrb.resval.rint=2;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return RTERROR;
	dp->trans(a,&ucsrb,&dcsrb,0,viewctr,NULL);
	viewctr[2]=0.0;
	return RTNORM;
	}

short db_header::SetViewctrAsIs(db_drawing *dp, sds_point a)
	{
	viewctr[0]=a[0];
	viewctr[1]=a[1];
	viewctr[2]=a[2];
	return RTNORM;
	}

void db_header::GetViewdir(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return;
	dp->trans(viewdir,&wcsrb,&ucsrb,1,a,NULL);
	}

short db_header::SetViewdir(db_drawing *dp, sds_point a)
	{
	struct resbuf wcsrb,ucsrb;
	wcsrb.restype=ucsrb.restype=RTSHORT;
	wcsrb.resval.rint=0;
	ucsrb.resval.rint=1;

	ASSERT(dp);
	if (dp==NULL)
		return RTERROR;
	if (icadRealEqual(a[0],0.0) && icadRealEqual(a[1],0.0) && icadRealEqual(a[2],0.0))
		return RTERROR;
	dp->trans(a,&ucsrb,&wcsrb,1,viewdir,NULL);
	return RTNORM;
	}

short db_header::SetViewdirAsIs(db_drawing *dp, sds_point a)
	{
	viewdir[0]=a[0];
	viewdir[1]=a[1];
	viewdir[2]=a[2];
	return RTNORM;
	}

short db_header::RetViewmode(db_drawing *dp)
	{
	return viewmode;
	}

short db_header::SetViewmode(db_drawing *dp, short a)
	{
	viewmode=a;
	return RTNORM;
	}

sds_real db_header::RetViewsize(db_drawing *dp)
	{
	return viewsize;
	}

short db_header::SetViewsize(db_drawing *dp, sds_real a)
	{
	viewsize=a;
	return RTNORM;
	}

sds_real db_header::RetViewtwist(db_drawing *dp)
	{
	return viewtwist;
	}

short db_header::SetViewtwist(db_drawing *dp, sds_real a)
	{
	viewtwist=a;
	return RTNORM;
	}

short db_header::RetVisretain(db_drawing *dp)
	{
	return visretain;
	}

short db_header::SetVisretain(db_drawing *dp, short a)
	{
	visretain=a;
	return RTNORM;
	}

void db_header::GetVsmax(db_drawing *dp, sds_point a)
	{
	a[0]=vsmax[0];
	a[1]=vsmax[1];
	a[2]=vsmax[2];
	}

short db_header::SetVsmax(db_drawing *dp, sds_point a)
	{
	vsmax[0]=a[0];
	vsmax[1]=a[1];
	vsmax[2]=a[2];
	return RTNORM;
	}

void db_header::GetVsmin(db_drawing *dp, sds_point a)
	{
	a[0]=vsmin[0];
	a[1]=vsmin[1];
	a[2]=vsmin[2];
	}

short db_header::SetVsmin(db_drawing *dp, sds_point a)
	{
	vsmin[0]=a[0];
	vsmin[1]=a[1];
	vsmin[2]=a[2];
	return RTNORM;
	}

short db_header::RetWorlducs(db_drawing *dp)
	{
	return worlducs;
	}

short db_header::SetWorlducs(db_drawing *dp, short a)
	{
	if (a!=0)
		worlducs=1;
	else
		worlducs=0;
	return RTNORM;
	}

short db_header::RetWorldview(db_drawing *dp)
	{
	return worldview;
	}

short db_header::SetWorldview(db_drawing *dp, short a)
	{
	worldview=a;
	return RTNORM;
	}

short db_header::RetWritestat(db_drawing *dp)
	{
	return writestat;
	}

short db_header::SetWritestat(db_drawing *dp, short a)
	{
	writestat=a;
	return RTNORM;
	}

short db_header::RetZoompercent(db_drawing *dp)
	{
	return zoompercent;
	}

short db_header::SetZoompercent(db_drawing *dp, short a)
	{
	zoompercent=a;
	return RTNORM;
	}
