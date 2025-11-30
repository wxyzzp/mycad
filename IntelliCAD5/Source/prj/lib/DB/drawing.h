/* C:\ICAD\PRJ\LIB\DB\DRAWING.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#ifndef	_DRAWING_H
#define	_DRAWING_H

// constants for Model Space Layout and Default Paper Space Layouts
const char MODELSPACE_LAYOUT[] = "Model";
const char PAPERSPACE_LAYOUT1[] = "Layout1";
const char PAPERSPACE_LAYOUT2[] = "Layout2";
 
/*
**	The "BIG" class for 1 whole drawing.
**
**							   PRIVATE
**
**	pn:
**
**	  Pathname.
**
**	fptr
**	  File pointer to this file ("rb" -- read/binary).
**
**	rel:
**
**	  ACAD release idx: 0=10; 1=11/12; 2=13/14
**	  This variable has actually been used to guide the reading
**	  of ACAD DWG FILES (determining how many tables are valid,
**	  how many read-buffers (rbuf[]) are needed (pre-R13 files
**	  need [1] for the Patch Table), etc).	See "ourver" for
**	  our file version code.  For ANY of ours, the current member (rel)
**	  should be set to 2 (R13's number of tables, only rbuf[0]
**	  allocated.)
**
**	rbuf[]:
**
**	  Drawing entity-reading buffers:
**
**		For R10 and R12:
**		  [0] = Entities section
**		  [1] = Other areas (Block definition and Patch sections)
**
**		For R13 and our file format:
**		  Only [0] is used.
**
**	header:
**
**	  The buffer for the drawing system variables.
**
**	sectseek[3][2]:
**
**	  Section seekers in pre-R13 DWGs:
**
**		[DRW_ENTSECT][] = Entity;	 |	[][0] = Beginning;
**		[DRW_BLKSECT][] = Block def; |	[][1] = 1st char after the end;
**		[DRW_PATSECT][] = Patch 	 |
**
**	table[]:
**
**	  An array covering data for all tables (including block defs,
**	  since they are part of the blocks table).  Allocated for
**	  DB_NTABS elements.
**
**	current.clayer
**			celtype
**			cmlstyle
**			dimstyle
**			dimtxsty
**			textstyle
**			ucsname
**			p_ucsname
**
**	  db_handitem pointers to the table records corresponding
**	  to the system variables that have the same names as the
**	  structure members.  getvar/setvar use these instead
**	  of the header (where they are R12-style table indices).
**	  The DWG readers/writers have to be aware of this and
**	  translate properly between the two forms.
**
**	hsort:
**
**	  Array of db_handitem pointers for qsort/bsearch by handle.
**	  (Re-allocated in DB_HSORTINC chunks, as needed.)
**
**	hsortsz:
**
**	  The number of elements hsort is allocated for.
**
**	nhsort:
**
**	  Number of elements currently USED in hsort.
**
**
**	entll[]:
**
**	  The llist of mspace and pspace entities that are not in
**	  block definitions.  [0] : the 1st link; [1] : last link (chaining).
**
**	lastmainent[]:
**
**	  Pointers to the last undeleted main entities for a particular space.
**
**		0 : MSPACE
**		1 : PSPACE
**		2 : The last in the database (in either space).
**
**	  (May be NULL when not currently known, in which case the code
**	  will have to go find them again by scanning entll[0].)
**
**	nmainents:
**
**	  The number of main entities successfully added to the drawing via
**	  db_drawing::addhanditem().  (Doesn't count block def entities.)
**
**	objll[]:
**
**	  The llist of db_handitems that ACAD calls OBJECTs (DICTIONARY,
**	  MLINESTYLE, etc.).  [0] : the 1st link; [1] : last link (chaining).
**
**
**	classll[]:
**
**	  The llist of CLASSs.	[0] : the 1st link; [1] : last link (chaining).
**
**	handoverflow:
**
**	  Set if handles are on and an overflow occurs.
**
**	pendflags:
**
**	  Bit-coded concerning pendentmake: xxxxxPIB
**
**		P : POLYLINE entity in progress
**		I : Complex INSERT entity in progress
**		B : Block def in progress
**
**	pendentmake[]:
**
**	  Llist of entities for a block def and/or complex entity
**	  whose entmaking is in progress.  [0] is the beginning link;
**	  [1] is the last link (for chaining purposes).
**
**	complexmain:
**
**	  The main entity link most recently encountered while
**	  adding to the database -- whether it's in the drawing's
**	  entll or pendentmake.  This is for the benefit of SEQEND,
**	  when it comes along.	Also used for 2D VERTEX elevation.
**
**	lastblockdef:
**
**	  The block table record most recently created OR modified
**	  by db_drawing::addhanditem() during a block (re)definition operation.
**
**	nextanon
**
**	  The next number to assign to an anonymous block name
**	  (*Xnn, *Dnn, or *Unn).
**
**	fontll:
**
**	  A llist of loaded fonts; the STYLE records have ptrs into this llist.
**	  (It is implemented as a stack, so no "last" pointer is needed.)
**
**	stockhand:
**
**	  An array of hex-string-format handles for MarComp.
**
**	imagebufbmp:
**	imagebufbmpDWGdirect:
**
**	  For the BMP image from the DWG for the thumbnail picture.
**
**	imagebufwmf:
**
**	  For the WMF image from the DWG for the thumbnail picture.
**
**
**	imagebufbmpsz and imagebufwmfsz:
**	imagebufbmpszDWGdirect:
**
**	  # of chars in the buffers.
**
**	sessionstarttime:
**
**	  The value returned by the C time() function when this object
**	  was created (the time at the start of this editing session).
**
**	usrstarttime:
**
**	  The value returned by the C time() function when USRTIMER was
**	  most recently toggled from OFF to ON.
**
**	freeit:
**
**	  A flag telling the destructor when to free the other members.
**	  (Kinda ridiculous.  Needed a way to create an uninitialized dummy
**	  object whose members would not get freed; see dummydp in
**	  makesvbuf().	C++ holds our hand way too hard, here.)
*/
class DB_CLASS db_drawing {  /* BIGDEF */

	friend int db_getvar(const char *vname, int qidx,		resbuf *res    , db_drawing *dp, db_charbuflink *configsv, db_charbuflink *sessionsv);
	friend int db_setvar(const char *vname, int qidx, const resbuf *argsour, db_drawing *dp, db_charbuflink *configsv, db_charbuflink *sessionsv, int asis);
	friend db_sysvarlink *db_findsysvar(const char *svname, int qidx, db_drawing *dp);
	friend int db_makesvbuf(int type, db_charbuflink *svbufp, db_drawing *dp);
	friend class drw_readwrite;

public:
	// EBATECH(CNBR) -[ 2002/9/19 Metric drawing from scratch
	db_drawing::db_drawing(int measureinit, int pstylepolicy);
	db_drawing(int mode);
	~db_drawing();

/*-------------------------------------------------------------------------*//**
Checks drawing objects for validity and fix errors if possible

@author Denis Petrov
@return 1 if objects is valid or successfully validated, 0 else or for error
*//*------------------------------------------------------------------------*/
	int check();

	int addDefLayoutDictAndRecordIfNotPresent();
	int addDefaultLayoutObject(db_handitem* pLayoutsDictHandItem, const char* pLayoutName);
	
	void copyheaderfrom(db_drawing* sour)
	{
		if(header.buf != NULL && header.size > 0 && sour->header.buf != NULL && sour->header.size == header.size)
			memcpy(header.buf, sour->header.buf, sour->header.size);
	}

	int			ret_FileVersion(void) { return m_nFileVersion; }
	void		init_FileVersion( int ver ) { m_nFileVersion = ver; }
	char		*ret_pn(void) { return pn; }
	int 		 ret_rel(void) { return rel; }
	char		*ret_headerbuf(void) { return header.buf; }
	db_handitem *ret_complexmain(void) { return complexmain; }
	db_handitem *ret_lastblockdef(void) { return lastblockdef; }
	db_objhandle ret_stockhand(int n) { return stockhand[n]; }
	char		*ret_imagebufbmp(void) { return imagebufbmp; }
	void		*ret_imagebufbmpDWGdirect(void) { return imagebufbmpDWGdirect; }
	char		*ret_imagebufwmf(void) { return imagebufwmf; }
	int 		 ret_imagebufbmpsz(void) { return imagebufbmpsz; }
	unsigned int ret_imagebufbmpszDWGdirect(void) { return imagebufbmpszDWGdirect; }
	int 		 ret_imagebufwmfsz(void) { return imagebufwmfsz; }

	int 		 ret_handoverflow(void) { return handoverflow; }
	void		 set_handoverflow(int p) { handoverflow=(p!=0); }

	int 		 ret_nmainents(void) { return nmainents; }
	void		 set_nmainents(int p)
				{
				  nmainents=p;
				  if ((p > 0) && (!isDirty)) set_isDirty(true);
				}

	bool		 ret_isDirty(void) { return isDirty; }
	void		 set_isDirty(bool dirtyState) {isDirty=dirtyState;}

	time_t		 ret_sessionstarttime(void) { return sessionstarttime; }
	time_t		 ret_usrstarttime(void) 	{ return usrstarttime;	   }
	void		 set_usrstarttime(void) 	{ time(&usrstarttime);	   }

	/*
	**	Apparently, it's possible for a LISP or ADS program to have
	**	an INSERT COMMAND interrupt an already active entmake
	**	(via separate threads?), so we need to be able to save
	**	and restore the "pending" stuff for addhanditem():
	*/
	void		 get_pendstuff(char 		 *pendflagsp,
							   db_handitem	**pendentmakep0,
							   db_handitem	**pendentmakep1,
							   db_handitem	**complexmainp,
							   db_handitem	**lastblockdefp) {

		/* pendflagsp should be the address (&) of a char. */
		/* The others should be the addresses (&s) of db_handitem pointers. */

		if (pendflagsp	 !=NULL) *pendflagsp   =pendflags;
		if (pendentmakep0!=NULL) *pendentmakep0=pendentmake[0];
		if (pendentmakep1!=NULL) *pendentmakep1=pendentmake[1];
		if (complexmainp !=NULL) *complexmainp =complexmain;
		if (lastblockdefp!=NULL) *lastblockdefp=lastblockdef;
	}
	void		 set_pendstuff(char 		pendflagsval,
							   db_handitem *pendentmakeval0,
							   db_handitem *pendentmakeval1,
							   db_handitem *complexmainval,
							   db_handitem *lastblockdefval) {

		pendflags	  =pendflagsval;
		pendentmake[0]=pendentmakeval0;
		pendentmake[1]=pendentmakeval1;
		complexmain   =complexmainval;
		lastblockdef  =lastblockdefval;
	}

	db_handitem *ret_firstent(void) { return entll[0]; }
	db_handitem *ret_lastent(void) { return entll[1]; }

	void get_entllends(db_handitem **firstpp, db_handitem **lastpp) {
		if (firstpp!=NULL) *firstpp=entll[0];
		if (lastpp !=NULL) *lastpp =entll[1];
	}
	void set_entllends(db_handitem *firstp, db_handitem *lastp) {
		entll[0]=firstp; entll[1]=lastp;
	}
	void nulllastmainent(void)
		{
		lastmainent[0]=lastmainent[1]=lastmainent[2]=NULL;
		}

	/*DG - 28.1.2002*/// Allow NULL args. Old code commented out below.
	void get_tabrecllends(int tabtype, db_handitem** firstpp, db_handitem** lastpp)
	{
		if(firstpp)
			*firstpp = NULL;
		if(lastpp)
			*lastpp = NULL;
		if(tabtype > -1 && tabtype < DB_NTABS)
		{
			if(firstpp)
				*firstpp = table[tabtype].m_pTabRecLL[0];
			if(lastpp)
				*lastpp = table[tabtype].m_pTabRecLL[1];
		}
	}
	/*
	void get_tabrecllends(int tabtype, db_handitem **firstpp, db_handitem **lastpp) {
		if (firstpp!=NULL && lastpp!=NULL) {
			*firstpp=*lastpp=NULL;
			if (tabtype>-1 && tabtype<DB_NTABS) {
				*firstpp=table[tabtype].m_pTabRecLL[0];
				*lastpp =table[tabtype].m_pTabRecLL[1];
			}
		}
	}
	*/
	void set_tabrecllends(int tabtype, db_handitem *firstp, db_handitem *lastp) {
		if (tabtype>-1 && tabtype<DB_NTABS) {
			table[tabtype].m_pTabRecLL[0]=firstp;
			table[tabtype].m_pTabRecLL[1]=lastp;
		}
	}
	void nulltblnext(int tabtype) {
		if (tabtype>-1 && tabtype<DB_NTABS) table[tabtype].m_pNextTabRec=NULL;
	}

	void get_objllends(db_handitem **firstpp, db_handitem **lastpp) {
		if (firstpp!=NULL) *firstpp=objll[0];
		if (lastpp !=NULL) *lastpp =objll[1];
	}
	void set_objllends(db_handitem *firstp, db_handitem *lastp) {
		objll[0]=firstp; objll[1]=lastp;
	}
	/* db_drawing::addhanditem() is the adder. */

	void get_classllends(db_class **firstpp, db_class **lastpp) {
		if (firstpp!=NULL) *firstpp=classll[0];
		if (lastpp !=NULL) *lastpp =classll[1];
	}
	void set_classllends(db_class *firstp, db_class *lastp) {
		classll[0]=firstp; classll[1]=lastp;
	}
	void addclass(db_class *p) {
		if (p!=NULL) {
			if (classll[0]==NULL) classll[0]	  =p;
			else				  classll[1]->next=p;
			classll[1]=p;
		}
	}

	void copyheader(void)		 { header.copybuf();	 }
	void restoreheader(void)	 { header.restorebuf();  }
	void freeheaderbufcopy(void) { header.freebufcopy(); }

	db_handitem *ret_currenttabrec(int qidx) {
		db_handitem *thip1=NULL;
		switch (qidx) {
			case DB_QCLAYER   : thip1=current.clayer   ; break;
			case DB_QCELTYPE  : thip1=current.celtype  ; break;
			case DB_QCMLSTYLE : thip1=current.cmlstyle ; break;
			case DB_QDIMSTYLE : thip1=current.dimstyle ; break;
			case DB_QDIMTXSTY : thip1=current.dimtxsty ; break;
			case DB_QTEXTSTYLE: thip1=current.textstyle; break;
			case DB_QUCSNAME  : thip1=current.ucsname  ; break;
			case DB_QP_UCSNAME: thip1=current.p_ucsname; break;
		}
		return thip1;
	}
	db_fontlink *ret_fontll(void) { return fontll; }

	int use_nextanon(int sourCount) {

		if(sourCount == 0)
			return (++nextanon);
		else if (sourCount > nextanon)
		{
			nextanon = sourCount;
			return (nextanon);
		}
		else 
			return sourCount;

	}

	void set_fontll(db_fontlink *p) { fontll=p; }
	void resetfonts(void);
	void set_stockhand(db_objhandle p, int n) {
		stockhand[n]=p;
	}
	void set_imagebufbmp(char *p, int sz) { imagebufbmp=p; imagebufbmpsz=sz; }
	// Updated to take a void* and unsigned int for use by DWGdirect.
	void set_imagebufbmpDWGdirect(void *p, int sz) { imagebufbmpDWGdirect=p; imagebufbmpszDWGdirect=sz; }
	void set_imagebufwmf(char *p, int sz) { imagebufwmf=p; imagebufwmfsz=sz; }

	int chk_fptr(void) {
		return (fptr==NULL && (pn==NULL || !*pn ||
				 (fptr=fopen(pn,"rb"/*DNT*/))==NULL));
	}
	void chk_rbufs(void) {
		/* Make sure rbuf(s) are allocated and force initial read */
		/* by making sure that any legit seek position generates */
		/* an idx beyond the buffer: */
		if (rbuf[0].buf==NULL) {
			rbuf[0].newbuf(DB_RBUFSZ,0);
			rbuf[0].seekof0=-rbuf[0].size-10L;
		}
		if (rel<2) {
			if (rbuf[1].buf==NULL) {
				rbuf[1].newbuf(DB_RBUFSZ,0);
				rbuf[1].seekof0=-rbuf[1].size-10L;
			}
		}
	}

	int entmake(struct resbuf *elist, db_handitem **hipp=NULL);
	int addhanditem(db_handitem *hip);
	int delhanditem(db_handitem *prevhip, db_handitem *hip, int mode, bool bDeleteObject = true);
	db_handitem *entnext_noxref(db_handitem *hip) {
	/*
	**	Returns a pointer to the handitem of the next non-deleted entity
	**	after hip -- or the first non-deleted entity when hip==NULL.
	*/
		for (hip=(hip==NULL) ? entll[0] : hip->next; hip!=NULL &&
			hip->ret_deleted(); hip=hip->next);
		if (hip!=NULL && hip->ret_type()==DB_ENDBLK) hip=NULL;
		return hip;
	}
	db_handitem *entlast(int which);  /* 0:MSPACE  1:PSPACE  2:Database */
	db_handitem *tblsearch(int tabtype, const char* recname, int setnext);
	db_handitem *tblnext(int tabtype, int rewind);

	db_handitem *findtabrec(int tabtype, const char *name, int validonly) {
		return (db_is_tabrectype(tabtype)) ?
			table[tabtype].findRecord(name, !!validonly) : NULL;
	}
	int ret_ntabrecs(int tabidx, int validonly) {
		int rc=0;
		db_handitem *thip1;

		if (tabidx>-1 && tabidx<DB_NTABS)
			for (thip1=table[tabidx].m_pTabRecLL[0]; thip1!=NULL; thip1=thip1->next)
				if (!validonly || !thip1->ret_deleted()) rc++;
		return rc;
	}
	db_handitem *ret_lasttabrec(int tabidx, int validonly) {
		db_handitem *thip1,*thip2;

		thip2=NULL;
		if (tabidx>-1 && tabidx<DB_NTABS)
			for (thip1=table[tabidx].m_pTabRecLL[0]; thip1!=NULL; thip1=thip1->next)
				if (!validonly || !thip1->ret_deleted()) thip2=thip1;
		return thip2;
	}
	db_handitem *namedobjdict(void) { return objll[0]; }
	int 		 dictadd(db_handitem *dicthip, char *recname, db_handitem *rechip, int HiRefType = DB_SOFT_OWNER);
	int 		 dictdel(db_handitem *dicthip, char *recname);
	int 		 dictrename(db_handitem *dicthip, char *oldname, char *newname);
	db_handitem *dictnext(db_handitem *dicthip, int rewind);
	db_handitem *icaddictnext(db_handitem *dicthip, int rewind);
	db_handitem *dictsearch(db_handitem *dicthip, char *recname, int setnext);

	bool blockdefpending(void)		{ return (pendflags&1) ? true : false; }
	bool blockdefonlypending(void)	{ return (pendflags==1) ? true : false; }
	bool complexinsertpending(void) { return (pendflags&2) ? true : false; }
	bool polylinepending(void)		{ return (pendflags&4) ? true : false; }

	void setblockdefpending(void)	   { pendflags |= 1; }
	void setcomplexinsertpending(void) { pendflags |= 2; }
	void setpolylinepending(void)	   { pendflags |= 4; }
	void setblockdefnotpending(void)	  { pendflags &= ~1; }
	void setcomplexinsertnotpending(void) { pendflags &= ~2; }
	void setpolylinenotpending(void)	  { pendflags &= ~4; }
	void setnothingpending(void)	   { pendflags = 0; }

	bool somethingpending(void) { return (pendflags & 7) ? true : false; }

	bool inpspace(void);

	db_handitem *db_drawing::handent(db_objhandle hand);

	void		 getucs(sds_point org, sds_point xdir, sds_point ydir, sds_point zdir);
	int 		 movept2elev(sds_point sour, sds_point dest);
	int 		 trans(sds_point pt, resbuf *from, resbuf *to, int disp, sds_point result, sds_point *altucs);
	int 		 initucs2ent(sds_point *xfa, sds_point extru, sds_real *dangp, short mode);
	int 		 ucs2ent(sds_point sour, sds_point dest, sds_point *xfa);

	int bufpos(long seeker, long howmuch, int *whichrbufp, long *bidxp);

	// These refer to MView viewports (floating)
	//
	db_viewport *GetViewport( int iVportNum );
	db_viewport *GetCurrentViewport( void );

// handling for tolerance font
	void set_cantfindtolfont(bool truefalse)
		{
		st_cantfindtol=truefalse;
		}

	bool ret_cantfindtolfont(void)
		{
		return(st_cantfindtol);
		}

	void set_tolfontptr(db_fontlink *fp)
		{
		st_tolfont=fp;
		}

	db_fontlink *ret_tolfontptr(void)
		{
		return(st_tolfont);
		}

	// This is needed so New can clear out a pathname picked up from a prototype drawing
	//
	bool ClearPathName( void )
		{
		db_astrncpy(&this->pn, ""/*DNT*/,-1);
		return true;
		}

	bool SetPathName( char *name )
		{
		db_astrncpy(&this->pn, name,-1);
		return true;
		}

	/* UNFINISHED FUNCTIONS: */
	int tblfixer(void);


	/* 1005 fixup table services */
	void AddHandleToFixupTable(db_objhandle handle, void **theptr);
	void AddHandleToTable(db_handitem *hip);
	bool Make1005FixupTables();
	bool FixingUp1005s();
	void Apply1005Fixups();
	void Delete1005FixupTables();
	bool GetNew1005Handle(const db_objhandle oldHandle, db_objhandle *newHandle);

	// EBATECH(CNBR) Bugzilla#78260
	// Some system variables are moved from/to DICTIONARYVAR/HEADER section.
	bool fixupDicVars(bool bSave, int version);

	// wblock anonymous name services
	void StartAnonBlkNameGen(void)
		{
		dontGenerateAnonNames = false;
		}
	void StopAnonBlkNameGen(void)
		{
		dontGenerateAnonNames = true;
		}
	bool GeneratingAnonNames(void)
		{
		return(!dontGenerateAnonNames);
		}

	bool WasLoadedFromA2K(void) { return m_loadedFromA2K; }
	void SetFileLoadedFromA2K(bool truefalse) { m_loadedFromA2K=truefalse; }

	void SetCopyingFlag(bool copying) {m_copyFlag = copying;}

	/* extension dictionary pointer handling functions */
	/* xdict keeps xdic for BLOCK_RECORD/*MODEL_SPACE and *PAPER_SPACE */
	void del_xdict(int p) {
		if(( p==0 || p==1 ) && xdict[p] != NULL ){ delete xdict[p]; xdict[p]=NULL; }
	}
	db_hireflink *ret_xdict(int p) {
		if( p==0 || p==1 ) return xdict[p]; else return NULL;
	}
	void set_xdict(int p, int type, db_objhandle hand, db_handitem *hip) {
		if( p==0 || p==1 ){ del_xdict(p); xdict[p] = new db_hireflink(type,hand,hip); }
	}


#ifdef NEWHEADERSTUFF
	db_header *RetNewHeader(void) { return &newheader; }
#endif

// Accessors for A2K only vbls

void set_dimfrac(short s)	{	dimfrac=s;	}
short ret_dimfrac(void)	{	return dimfrac;	}
void set_dimtmove(short s)	{	dimtmove=s;	}
short ret_dimtmove(void)	{	return dimtmove;	}
void set_endcaps(char s)	{	endcaps=s;	}
char ret_endcaps(void)	{	return endcaps;	}
void set_joinstyle(char s)	{	joinstyle=s;	}
char ret_joinstyle(void)	{	return joinstyle;	}
void set_tducreate(long l[2])	{	tducreate[0]=l[0]; tducreate[1]=l[1];	}
void get_tducreate(long l[2])	{	l[0]=tducreate[0]; l[1]=tducreate[1];	}
void set_tduupdate(long l[2])	{	tduupdate[0]=l[0]; tduupdate[1]=l[1];	}
void get_tduupdate(long l[2])	{	l[0]=tduupdate[0]; l[1]=tduupdate[1];	}
void set_dimldrblk(char *s)	{	strcpy(dimldrblk,s);	}
void get_dimldrblk(char *s)	{	strcpy(s,dimldrblk);	}
void set_fingerprintguid(char *s)	{	strcpy(fingerprintguid,s);	}
void get_fingerprintguid(char *s)	{	strcpy(s,fingerprintguid);	}
void set_versionguid(char *s)	{	strcpy(versionguid,s);	}
void get_versionguid(char *s)	{	strcpy(s,versionguid);	}
void set_pucsorgback(sds_point p)	{	pucsorgback[0]=p[0];	pucsorgback[1]=p[1];	pucsorgback[2]=p[2];	}
void get_pucsorgback(sds_point p)	{	p[0]=pucsorgback[0];	p[1]=pucsorgback[1];	p[2]=pucsorgback[2];	}
void set_pucsorgbottom(sds_point p)	{	pucsorgbottom[0]=p[0];	pucsorgbottom[1]=p[1];	pucsorgbottom[2]=p[2];	}
void get_pucsorgbottom(sds_point p)	{	p[0]=pucsorgbottom[0];	p[1]=pucsorgbottom[1];	p[2]=pucsorgbottom[2];	}
void set_pucsorgfront(sds_point p)	{	pucsorgfront[0]=p[0];	pucsorgfront[1]=p[1];	pucsorgfront[2]=p[2];	}
void get_pucsorgfront(sds_point p)	{	p[0]=pucsorgfront[0];	p[1]=pucsorgfront[1];	p[2]=pucsorgfront[2];	}
void set_pucsorgleft(sds_point p)	{	pucsorgleft[0]=p[0];	pucsorgleft[1]=p[1];	pucsorgleft[2]=p[2];	}
void get_pucsorgleft(sds_point p)	{	p[0]=pucsorgleft[0];	p[1]=pucsorgleft[1];	p[2]=pucsorgleft[2];	}
void set_pucsorgright(sds_point p)	{	pucsorgright[0]=p[0];	pucsorgright[1]=p[1];	pucsorgright[2]=p[2];	}
void get_pucsorgright(sds_point p)	{	p[0]=pucsorgright[0];	p[1]=pucsorgright[1];	p[2]=pucsorgright[2];	}
void set_pucsorgtop(sds_point p)	{	pucsorgtop[0]=p[0];	pucsorgtop[1]=p[1];	pucsorgtop[2]=p[2];	}
void get_pucsorgtop(sds_point p)	{	p[0]=pucsorgtop[0];	p[1]=pucsorgtop[1];	p[2]=pucsorgtop[2];	}
void set_ucsorgback(sds_point p)	{	ucsorgback[0]=p[0];	ucsorgback[1]=p[1];	ucsorgback[2]=p[2];	}
void get_ucsorgback(sds_point p)	{	p[0]=ucsorgback[0];	p[1]=ucsorgback[1];	p[2]=ucsorgback[2];	}
void set_ucsorgbottom(sds_point p)	{	ucsorgbottom[0]=p[0];	ucsorgbottom[1]=p[1];	ucsorgbottom[2]=p[2];	}
void get_ucsorgbottom(sds_point p)	{	p[0]=ucsorgbottom[0];	p[1]=ucsorgbottom[1];	p[2]=ucsorgbottom[2];	}
void set_ucsorgfront(sds_point p)	{	ucsorgfront[0]=p[0];	ucsorgfront[1]=p[1];	ucsorgfront[2]=p[2];	}
void get_ucsorgfront(sds_point p)	{	p[0]=ucsorgfront[0];	p[1]=ucsorgfront[1];	p[2]=ucsorgfront[2];	}
void set_ucsorgleft(sds_point p)	{	ucsorgleft[0]=p[0];	ucsorgleft[1]=p[1];	ucsorgleft[2]=p[2];	}
void get_ucsorgleft(sds_point p)	{	p[0]=ucsorgleft[0];	p[1]=ucsorgleft[1];	p[2]=ucsorgleft[2];	}
void set_ucsorgright(sds_point p)	{	ucsorgright[0]=p[0];	ucsorgright[1]=p[1];	ucsorgright[2]=p[2];	}
void get_ucsorgright(sds_point p)	{	p[0]=ucsorgright[0];	p[1]=ucsorgright[1];	p[2]=ucsorgright[2];	}
void set_ucsorgtop(sds_point p)	{	ucsorgtop[0]=p[0];	ucsorgtop[1]=p[1];	ucsorgtop[2]=p[2];	}
void get_ucsorgtop(sds_point p)	{	p[0]=ucsorgtop[0];	p[1]=ucsorgtop[1];	p[2]=ucsorgtop[2];	}
void set_pucsbase(db_objhandle oh)	{	pucsbase=oh;	}
db_objhandle ret_pucsbase(void)	{	return pucsbase;	}
void set_pucsorthoref(db_objhandle oh)	{	pucsorthoref=oh;	}
db_objhandle ret_pucsorthoref(void)	{	return pucsorthoref;	}
void set_ucsbase(db_objhandle oh)	{	ucsbase=oh;	}
db_objhandle ret_ucsbase(void)	{	return ucsbase;	}
void set_ucsorthoref(db_objhandle oh)	{	ucsorthoref=oh;	}
db_objhandle ret_ucsorthoref(void)	{	return ucsorthoref;	}
// end of A2K only accessors

public:
							// pointer to application undo list
	class AbstractDelete	*m_undoList;

  private:

	char			 *pn;
	FILE			 *fptr;
	char			  rel;

	db_charbuflink	  rbuf[2];

	db_charbuflink	  header;

	long			  sectseek[3][2];

	db_table		 *table;

	struct {
		db_handitem  *clayer;
		db_handitem  *celtype;
		db_handitem  *cmlstyle;
		db_handitem  *dimstyle;
		db_handitem  *dimtxsty;
		db_handitem  *textstyle;
		db_handitem  *ucsname;
		db_handitem  *p_ucsname;
	} current;

	db_handitem 	 *entll[2];
	db_handitem 	 *lastmainent[3];
	int 			  nmainents;
	bool			  isDirty;

	db_handitem 	 *objll[2];

	db_class		 *classll[2];

	int 			  handoverflow;

	char			  pendflags;
	db_handitem 	 *pendentmake[2];

	db_handitem 	 *complexmain;
	db_handitem 	 *lastblockdef;

	int 			  nextanon;
	bool			  dontGenerateAnonNames;

	db_fontlink 	 *fontll;

	db_objhandle	  stockhand[DB_NSTOCKHANDS];

	char			 *imagebufbmp;
	void			 *imagebufbmpDWGdirect;
	char			 *imagebufwmf;

	int 			  imagebufbmpsz;
	unsigned int	  imagebufbmpszDWGdirect;
	int 			  imagebufwmfsz;

	time_t			  sessionstarttime;
	time_t			  usrstarttime;

	char			  freeit;

	class HandleTable	*handleTable;
	class FixupTable	*fixupTable;
	class FailedFontTable		*failedFontTable;  // list of fonts we already tried to find, but couldn't
	class FixupHandleTable *m_fixup1005HandleMap;
	class Fixup1005HipTable *m_fixup1005HipTable;
	bool  m_loadedFromA2K;
	int	  m_nFileVersion;

	bool st_cantfindtol;
	db_fontlink *st_tolfont;

	// When copying do not want to create the associations.  Pointers will be resolved after
	// copy.
	bool m_copyFlag;

	// Ownership ptr to extension dictionary
	// [0] BLOCK_RECORD|*MODEL_SPACE [1] BLOCK_RECORD|*PAPER_SPACE
	db_hireflink *xdict[2];


// A2K-only variables which are round-tripped, not used
	short dimfrac;
	// ]-EBATECH(watanabe)
	char dimldrblk[256];
	short dimtmove;
	char endcaps;
	char fingerprintguid[128];
	char joinstyle;
	db_objhandle pucsbase;	// EBATECH(CNBR) Cmnt It will be a handitem.
	sds_point pucsorgback;
	sds_point pucsorgbottom;
	sds_point pucsorgfront;
	sds_point pucsorgleft;
	sds_point pucsorgright;
	sds_point pucsorgtop;
	db_objhandle pucsorthoref;
	long tducreate[2];
	long tduupdate[2];
	db_objhandle ucsbase;
	sds_point ucsorgback;
	sds_point ucsorgbottom;
	sds_point ucsorgfront;
	sds_point ucsorgleft;
	sds_point ucsorgright;
	sds_point ucsorgtop;
	db_objhandle ucsorthoref;
	char versionguid[128];
// end of A2K only variables

#ifdef NEWHEADERSTUFF
	class db_header		newheader;
#endif
};

#endif		// _DRAWING_H

