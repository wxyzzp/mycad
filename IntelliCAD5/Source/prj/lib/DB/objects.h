/* C:\ICAD\PRJ\LIB\DB\OBJECTS.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#ifndef _INC_OBJECTS
#define _INC_OBJECTS

class DB_CLASS db_acad_proxy_object : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_acad_proxy_object(void);
	db_acad_proxy_object(const db_acad_proxy_object &sour);  /* Copy constructor */
   ~db_acad_proxy_object(void);  /* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	//EBATECH(CNBR) -[ 2002/10/10 Bugzilla78218
	//int  ret_entid(void)	{ return entid; }
	//void set_entid(int p) { entid=p; }

	void get_hirefllends(db_hireflink **firstpp, db_hireflink **lastpp) {
		if (firstpp!=NULL) *firstpp=hirefll[0];
		if (lastpp !=NULL) *lastpp =hirefll[1];
	}
	void set_hirefllends(db_hireflink *firstp, db_hireflink *lastp) {
		hirefll[0]=firstp; hirefll[1]=lastp;
	}

	int ret_nhirefs(void) {
		int rc;
		db_hireflink *tp;

		for (tp=hirefll[0],rc=0; tp!=NULL; tp=tp->next,rc++);
		return rc;
	}

	void addhiref(db_hireflink *toadd) {
		if (toadd!=NULL) {
			if (hirefll[0]==NULL) hirefll[0]=toadd; else hirefll[1]->next=toadd;
			hirefll[1]=toadd;
		}
	}

	bool get_90(int *p) { *p=pid	 ; return true; }
	bool get_91(int *p) { *p=aid	 ; return true; }

	bool set_90(int p) { pid=p	   ; return true; }
	bool set_91(int p) { aid=p	   ; return true; }
	bool get_95(int *p) { *p=objectdrawingformat; return true;}
	bool get_70(int *p) { *p=origdataformat; return true;}
	bool set_95(int p) { objectdrawingformat=p; return true;}
	bool set_70(int p) { origdataformat=p; return true;}
	// Note: db_acad_proxy_object has entgespecific and entmod.
	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp) { return 0; }

	void  setResbufChain(struct sds_resbuf* p) { m_pResbufChain = p; }
	struct sds_resbuf* getResbufChain() { return m_pResbufChain; }
	void  setOdDbClass(void* p) { m_OdClass = p; }
	void* getOdDbClass() { return m_OdClass; }

  private:

	char		  deleted;

	int 		  pid;		   /*  90 */
	int 		  aid;		   /*  91 The DWG internal ent type */
	db_hireflink *hirefll[2];  /* 330-360s */
	int 		 objectdrawingformat; /* 95 */
	int 		 origdataformat; /* 70 */

	// Holds the data for an OdDb-data.
	struct sds_resbuf* m_pResbufChain;
	void* m_OdClass;
};


/*
**	THE DICTNEXT PROBLEM:
**
**	In ACAD, make a DICTIONARY that has the following ename pattern
**	in its records:
**
**		en0
**		en1
**		en2
**		en1
**
**	Using dictnext repeatly, you get the following infinite loop:
**
**		en0,en1,en2,en1,en2,en1,en2,...
**
**	ACAD's dictnext apparently finds the FIRST OCCURRENCE of the ename
**	it's going to return and then sets the pointer to the one after
**	that for next time.  That is, it focuses on the enames --
**	not the DICTIONARY records.  (This may indicate a fundamental
**	difference between the way they and we have implemented
**	the database.)
**
**	Therefore, we have two "dictnext" functions:
**
**		dictnext() to match ACAD's
**		icaddictnext() for us (which works the way you'd expect).
*/
class db_dictionaryrec {

  public:

	db_dictionaryrec(void) 
	{ 
		name=NULL; 
		hiref=NULL; 
		next=NULL; 
	}

	db_dictionaryrec(const char *pname, int hireftype, db_objhandle hirefhand,
		db_handitem *hirefhip) 
	{

		next=NULL;
		name=NULL; db_astrncpy(&name,pname,-1); 
		if (name!=NULL) 
			_tcsupr(name);
		hiref=new db_hireflink(hireftype,hirefhand,hirefhip);
	}

	db_dictionaryrec(const db_dictionaryrec &sour) /* Copy constructor */
	{  
		next=NULL;
		name=NULL; 
		db_astrncpy(&name,sour.name,-1);
		hiref=new db_hireflink(*sour.hiref);
	}

   ~db_dictionaryrec(void) 
   { 
	   delete [] name; 
	   delete hiref; 
   }

	char		 *name;   /*   3 */
	db_hireflink *hiref;  /* 350 */

	db_dictionaryrec *next;
};

class DB_CLASS db_dictionary : public db_handitem 
{
	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

public:

	db_dictionary();
	db_dictionary(const db_dictionary &sour);  /* Copy constructor */
	~db_dictionary();  /* Implicitly virtual */

	virtual int check(db_drawing* pDrawing);
	int freeIndex() { return ++m_index; }
	int ret_deleted(void) { return m_deleted; }
	void set_deleted(int p) { m_deleted = (char)(p!=0); }

	void get_recllends(db_dictionaryrec **firstpp, db_dictionaryrec **lastpp) {
		if (firstpp!=NULL) *firstpp=recll[0];
		if (lastpp !=NULL) *lastpp =recll[1];
	}
	void set_recllends(db_dictionaryrec *firstp, db_dictionaryrec *lastp) {
		recll[0]=firstp; recll[1]=lastp;
	}

	db_dictionaryrec *ret_acadnextrec(void) { return acadnextrec; }
	db_dictionaryrec *ret_icadnextrec(void) { return icadnextrec; }
	void set_acadnextrec(db_dictionaryrec *p) { acadnextrec=p; }
	void set_icadnextrec(db_dictionaryrec *p) { icadnextrec=p; }

	int ret_nrecs(void) {
		int rc;
		db_dictionaryrec *tp;

		for (tp=recll[0],rc=0; tp!=NULL; tp=tp->next,rc++);
		return rc;
	}

	void stepacadnextrec(db_drawing *dp) {	/* ACAD-style.	See comments above. */
		db_handitem *thip1;

		if (acadnextrec==NULL || acadnextrec->hiref==NULL ||
			acadnextrec->hiref->ret_hip(dp)==NULL) {

			acadnextrec=NULL;
		} else {
			thip1=acadnextrec->hiref->ret_hip(dp);
			for (acadnextrec=recll[0]; acadnextrec!=NULL &&
				(acadnextrec->hiref==NULL ||
				acadnextrec->hiref->ret_hip(dp)==NULL ||
				acadnextrec->hiref->ret_hip(dp)!=thip1); acadnextrec=acadnextrec->next);
			acadnextrec=(acadnextrec==NULL) ? NULL : acadnextrec->next;
		}
	}
	void stepicadnextrec(void) {  /* OURS.	See comments above. */
		icadnextrec=(icadnextrec==NULL) ? NULL : icadnextrec->next;
	}

	void addrec(db_dictionaryrec* toadd)
	{
		if(toadd)
		{
			if(!recll[0])
				recll[0] = acadnextrec = icadnextrec = toadd;
			else
				recll[1]->next = toadd;
			recll[1] = toadd;

			/*DG - 14.2.2002*/// We still don't implemented reactors completely, so this code
			// has to be commented out. And it must be done on 6.2.2002 together with others.
//			/*DG - 24.1.2002*/// A desirable thing, because a record's hiref's item may not have yet a soft pointer to this dic.
//			if(toadd->hiref && toadd->hiref->retp_hip() && *toadd->hiref->retp_hip())
//				(*toadd->hiref->retp_hip())->addReactor(RetHandle(), this, ReactorItem::SOFT_POINTER);
		}
	}

	int delrec(char* recname)
	{
	//	Returns:
	//		 0 : OK
	//		-1 : recname is NULL or ""
	//		-2 : recname not found

		db_dictionaryrec	*tp1 = NULL, *tp2 = recll[0];

		if(!recname || !*recname)
			return -1;

		for( ; tp2 && (!tp2->name || db_compareNames(tp2->name, recname)); tp1 = tp2, tp2 = tp2->next)
			;

		if(!tp2)
			return -2;

		if(!tp1)
			recll[0] = tp2->next;
		else
			tp1->next = tp2->next;
		if(!tp2->next)
			recll[1] = tp1;

		if(acadnextrec == tp2)
			acadnextrec = tp2->next;
		if(icadnextrec == tp2)
			icadnextrec = tp2->next;

		/*DG - 6.2.2002*/// We still don't implemented reactors completely, so this code
		// has to be commented out.
//		/*DG - 24.1.2002*/// A necessary thing, because a record's hiref's item may have a soft pointer to this dic.
//		if(tp2->hiref && tp2->hiref->retp_hip() && *tp2->hiref->retp_hip())
//			(*tp2->hiref->retp_hip())->removeReactor(RetHandle());

		delete tp2;
		return 0;
	}

	db_dictionaryrec *findrec(const char *name, int setnext, db_drawing *dp) {
		db_dictionaryrec *found=NULL;

		if (name!=NULL && *name) {
			for(found = recll[0]; found != NULL; found = found->next)
			{
				if(found->hiref->ret_hip(dp)->ret_deleted())
					continue;
				if(found->name != NULL && !db_compareNames(found->name,name))
					break;
			}
		}
		if (setnext && found!=NULL) {
			acadnextrec=icadnextrec=found;
			stepacadnextrec(dp); stepicadnextrec();
		}
		return found;
	}
	int renamerec(char *oldname, char * newname, db_drawing *dp) {
		db_dictionaryrec *thisrec;

		if (oldname==NULL || !*oldname ||
			newname==NULL || !*newname) 	 return -1;  /* Param NULL or "" */
		if ((thisrec=findrec(oldname,0,dp))==NULL) return -2;  /* Can't find old */
		if (findrec(newname,0,dp)!=NULL)		   return -3;  /* New already exists. */
		db_astrncpy(&thisrec->name,newname,-1);
		if (thisrec->name!=NULL) _tcsupr(thisrec->name);
		return 0;
	}

	/*
	**	Normally, this function would be a static member function
	**	of db_dictionaryrec, for the deletion of ANY llists of them.
	**	But since acadnextrec and icadnextrec must be NULLed and since
	**	this class is the only place likely to use a llist of them,
	**	I'm putting it here:
	*/
	void delrecll()
	{
		while(recll[0])
		{
			recll[1] = recll[0]->next;

			/*DG - 6.2.2002*/// We still don't implemented reactors completely, so this code
			// has to be commented out.
//			/*DG - 24.1.2002*/// A necessary thing, because a record's hiref's item may have a soft pointer to this dic.
//			if(recll[0]->hiref && recll[0]->hiref->retp_hip() && *recll[0]->hiref->retp_hip())
//				(*recll[0]->hiref->retp_hip())->removeReactor(RetHandle());

			delete recll[0];
			recll[0] = recll[1];
		}
		recll[0] = recll[1] = acadnextrec = icadnextrec = NULL;
	}

	// Note: db_dictionary has entgespecific and entmod.
	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp);
	// EBATECH(CNBR) -[ 2002/6/30 Bugzilla78229 DXF=280 and 281
	//short ret_cloningflag(void) { return cloningflag; }
	//void set_cloningflag(short s) { cloningflag=s; }
	bool get_280(int *p) { *p=hardownerflag; return true; }
	bool get_281(int *p) { *p=cloningflag; return true; }
	bool set_280(int p) { hardownerflag = ((p == 0 || p != 1 ) ? p : 0); return true; }
	bool set_281(int p) { cloningflag = (( p < 0 || p > 5 ) ? 1 : p); return true; }
	// EBATECH(CNBR) ]-

private:
	char m_deleted;
	int m_index;

	/* See comments above for an explanation of these two: */
	db_dictionaryrec *acadnextrec;	/* Ptr into recll for ACAD-style "dictnext". */
	db_dictionaryrec *icadnextrec;	/* Ptr into recll for OUR "dictnext". */

	db_dictionaryrec *recll[2];  /* llist of records */
	short hardownerflag;	/* 280 EBATECH(CNBR) 2002/6/30 Bugzilla78229 */
	short cloningflag;	/* 281 */
};


class DB_CLASS db_dictionaryvar : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_dictionaryvar(void);
	db_dictionaryvar(const db_dictionaryvar &sour);  /* Copy constructor */
   ~db_dictionaryvar(void);  /* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	// EBATECH(CNBR) -[ 2002/10/10 Bugzilla78233 DXF=280 and 1
	//int	ret_unkint(void)   { return unkint;  }
	//char *ret_unkstr(void)   { return unkstr;  }
	//void set_unkint(int p)   { unkint =p; }
	//void set_unkstr(char *p) { db_astrncpy(&unkstr,p,-1); }
	char *ret_1(void) {return unkstr; }
	bool get_1(char *p, int maxlen) {
		if (unkstr==NULL) *p=0;
		else { strncpy(p,unkstr,maxlen); p[maxlen]=0; }
		return true;
	}
	bool get_1(char **p) { *p=unkstr; return true; }
	bool get_280(int *p) { *p=unkint; return true; }
	bool set_1(char *p) {
		db_astrncpy(&unkstr,(p==NULL) ? db_str_quotequote : p,-1);
		return true;
	}
	bool set_280(int p) { unkint=p; return true; }
	// Note: db_dictionaryvar is not necessary entgespecific and entmod.
	// EBATECH(CNBR) ]-

  private:

	char  deleted;

	/* Totally based on MarComp: */
	int   unkint;	// 280 schema
	char *unkstr;	// 1 val
};

class DB_CLASS db_idbuffer : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_idbuffer(void);
	db_idbuffer(const db_idbuffer &sour);  /* Copy constructor */
   ~db_idbuffer(void);	/* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	void get_hirefllends(db_hireflink **firstpp, db_hireflink **lastpp) {
		if (firstpp!=NULL) *firstpp=hirefll[0];
		if (lastpp !=NULL) *lastpp =hirefll[1];
	}
	void set_hirefllends(db_hireflink *firstp, db_hireflink *lastp) {
		hirefll[0]=firstp; hirefll[1]=lastp;
	}

	int ret_nhirefs(void) {
		int rc;
		db_hireflink *tp;

		for (tp=hirefll[0],rc=0; tp!=NULL; tp=tp->next,rc++);
		return rc;
	}

	void addhiref(db_hireflink *toadd) {
		if (toadd!=NULL) {
			if (hirefll[0]==NULL) hirefll[0]=toadd; else hirefll[1]->next=toadd;
			hirefll[1]=toadd;
		}
	}

	void delhirefll(void) {
		while (hirefll[0]!=NULL)
			{ hirefll[1]=hirefll[0]->next; delete hirefll[0]; hirefll[0]=hirefll[1]; }
		hirefll[0]=hirefll[1]=NULL;
	}

	// EBATECH(CNBR) -[ 2002/10/10 Bugzilla78218
	// Note: db_idbuffer has entgespecific and entmod.
	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	// EBATECH(CNBR) ]-
	int entmod(struct resbuf *modll, db_drawing *dp);

  private:

	char  deleted;

	/* Totally based on MarComp: */

	db_hireflink *hirefll[2];	/*	330 */
};



class DB_CLASS db_imagedef : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_imagedef(void);
	db_imagedef(const db_imagedef &sour);  /* Copy constructor */
   ~db_imagedef(void);	/* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	void *ret_imagedisp(void)	 { return imagedisp; }
	void  set_imagedisp(void *p) { imagedisp=p; }

	int  ret_looked4image(void) { return looked4image; }
	void set_looked4image(int p) { looked4image=(char)p; }

	bool get_1(char **p)
		{
		*p=filepn;
		return true;
		}
	bool get_10(sds_point p) {
		p[0]=imagesz[0]; p[1]=imagesz[1]; p[2]=0.0; return true;
	}
	bool get_11(sds_point p) {
		p[0]=dxf11[0]; p[1]=dxf11[1]; p[2]=0.0; return true;
	}
	bool get_90(int *p)  { *p=dxf90;  return true; }
	bool get_280(int *p) { *p=dxf280; return true; }
	bool get_281(int *p) { *p=dxf281; return true; }

	bool set_1(char *p)
		{
		db_astrncpy(&filepn,p,-1);
		return true;
		}
	bool set_10(sds_point p) { imagesz[0]=p[0]; imagesz[1]=p[1]; return true; }
	bool set_11(sds_point p) { dxf11[0]  =p[0]; dxf11[1]  =p[1]; return true; }
	bool set_90(int p) { dxf90=p; return true; }
	bool set_280(int p) { dxf280=(char)p; return true; }
	bool set_281(int p) { dxf281=(char)p; return true; }

	// Note: db_imagedef is not necessary entgespecific and entmod.

  private:

	char  deleted;

	char	  looked4image;  /* To avoid endlessly searching for filepn */
							 /* (similar to db_block::looked4xref). */

	char	 *filepn;	   /*	1 */
	sds_real  imagesz[2];  /*  10 */
	sds_real  dxf11[2];    /*  11 */
	int 	  dxf90;	   /*  90 */
	char	  dxf280;	   /* 280 */
	char	  dxf281;	   /* 281 */

	void	 *imagedisp;   /* Display data for and image def */
};


class DB_CLASS db_imagedef_reactor : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_imagedef_reactor(void);
	db_imagedef_reactor(const db_imagedef_reactor &sour);  /* Copy constructor */
   ~db_imagedef_reactor(void);	/* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	void get_hirefllends(db_hireflink **firstpp, db_hireflink **lastpp) {
		if (firstpp!=NULL) *firstpp=hirefll[0];
		if (lastpp !=NULL) *lastpp =hirefll[1];
	}
	void set_hirefllends(db_hireflink *firstp, db_hireflink *lastp) {
		hirefll[0]=firstp; hirefll[1]=lastp;
	}

	int ret_nhirefs(void) {
		int rc;
		db_hireflink *tp;

		for (tp=hirefll[0],rc=0; tp!=NULL; tp=tp->next,rc++);
		return rc;
	}

	void addhiref(db_hireflink *toadd) {
		if (toadd!=NULL) {
			if (hirefll[0]==NULL) hirefll[0]=toadd; else hirefll[1]->next=toadd;
			hirefll[1]=toadd;
		}
	}

	void delhirefll(void) {
		while (hirefll[0]!=NULL)
			{ hirefll[1]=hirefll[0]->next; delete hirefll[0]; hirefll[0]=hirefll[1]; }
		hirefll[0]=hirefll[1]=NULL;
	}

	bool get_90(int *p)  { *p=classver; return true; }

	bool set_90(int p) { classver=p; return true; }

	// Note: db_imagedef_reactor has entgespecific and entmod.
	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp);

  private:

	char  deleted;

	int 		  classver;    /*  90  (ACAD docs say 2=R14) */
	db_hireflink *hirefll[2];  /* 330 */
};


class db_layer_index : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_layer_index(void);
	db_layer_index(const db_layer_index &sour);  /* Copy constructor */
   ~db_layer_index(void);  /* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	int  ret_nrecs(void)  { return nrecs; }
	void set_nrecs(int p) { nrecs=p; }

	int  ret_timeval0(void)  { return timeval[0]; }
	int  ret_timeval1(void)  { return timeval[1]; }
	void set_timeval0(int p) { timeval[0]=p; }
	void set_timeval1(int p) { timeval[1]=p; }

	char *ret_data(void)   { return data; }
	int   ret_datasz(void) { return datasz; }

	void set_data(char *pdata, int pdatasz) {  /* Also sets datasz. */
		delete [] data; data=NULL; datasz=0;
		if (pdata!=NULL && pdatasz>0) {
			datasz=pdatasz; data=new char[datasz]; memcpy(data,pdata,datasz);
		}
	}

	char *ret_handdata(void)   { return handdata; }
	int   ret_handdatasz(void) { return handdatasz; }

	void set_handdata(char *phanddata, int phanddatasz) {  /* Also sets handdatasz. */
		delete [] handdata; handdata=NULL; handdatasz=0;
		if (phanddata!=NULL && phanddatasz>0) {
			handdatasz=phanddatasz; handdata=new char[handdatasz];
			memcpy(handdata,phanddata,handdatasz);
		}
	}

	// Note: db_layer_index cannot update its data now.
	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp) { return 0; }

	void  setResbufChain(struct sds_resbuf* p) { m_pResbufChain = p; }
	struct sds_resbuf* getResbufChain() { return m_pResbufChain; }
	void  setOdDbClass(void* p) { m_OdClass = p; }
	void* getOdDbClass() { return m_OdClass; }

  private:

	char  deleted;

	/* Totally based on MarComp: */

	int   nrecs;
	int   timeval[2];
	int   datasz;
	char *data;
	int   handdatasz;
	char *handdata;

	// Holds the data for an OdDb-data.
	struct sds_resbuf* m_pResbufChain;
	void* m_OdClass;
};


class db_mlselement {

  public:

	db_mlselement(void) {
		lthip=NULL; offset=0.0; color=DB_BLCOLOR;
	}

	db_handitem *lthip;   /*   6 */  /* Don't free; just a reference. */
	sds_real	 offset;  /*  49 */
	int 		 color;   /*  62 */
};

class DB_CLASS db_mlinestyle : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_mlinestyle(void);
	db_mlinestyle(const db_mlinestyle &sour);  /* Copy constructor */
   ~db_mlinestyle(void);  /* Implicitly virtual */

	int  ret_deleted(void)	{ return deleted; }
	void set_deleted(int p) { deleted=(char)(p!=0); }

	bool get_2(char 	 **p) { *p=name 	; return true; }
	bool get_3(char 	 **p) { *p=desc 	; return true; }
	bool get_51(sds_real  *p) { *p=ang[0]	; return true; }
	bool get_52(sds_real  *p) { *p=ang[1]	; return true; }
	bool get_62(int 	  *p) { *p=fillcolor; return true; }
	bool get_70(int 	  *p) { *p=flags	; return true; }
	bool get_71(int 	  *p) { *p=nelems	; return true; }

	// Note: db_mlinestyle has entgetspecific and entmod.
	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp);

	/* Here are the array getters for the elements: */
	bool get_6(char **p, int n) {  /* Sets an array of char ptrs. */
		int fi1;
		if (p!=NULL) {
			for (fi1=0; fi1<n; fi1++) {
				if (fi1>=nelems || elem[fi1].lthip==NULL) p[fi1]=NULL;
				else elem[fi1].lthip->get_2(p+fi1);
			}
		}
		return true;
	}
	bool get_6(db_handitem **p, int n) {  /* Sets an array of char ptrs. */
		int fi1;
		if (p!=NULL)
			for (fi1=0; fi1<n; fi1++)
				p[fi1]=(fi1<nelems) ? elem[fi1].lthip : NULL;
		return true;
	}
	bool get_49(sds_real *p, int n) {
		int fi1;
		if (p!=NULL) {
			for (fi1=0; fi1<n; fi1++)
				p[fi1]=(fi1<nelems) ? elem[fi1].offset : 0.0;
		}
		return true;
	}
	bool get_62(int *p, int n) {
		int fi1;
		if (p!=NULL) {
			for (fi1=0; fi1<n; fi1++)
				p[fi1]=(fi1<nelems) ? elem[fi1].color : DB_BLCOLOR;
		}
		return true;
	}


	bool set_2(char* p) {
		db_astrncpy(&name,(p==NULL) ? db_str_quotequote : p,-1);
		return true;
	}
	bool set_3(char *p) {
		db_astrncpy(&desc,(p==NULL) ? db_str_quotequote : p,-1);
		return true;
	}
	bool set_51(sds_real p) { ang[0]=p; return true; }
	bool set_52(sds_real p) { ang[1]=p; return true; }
	bool set_62(int p) { fillcolor=p; return true; }
	bool set_70(int p) { flags=p; return true; }

	/* Here are the array setters for the elements: */
	bool set_71(int p) {  /* CALL FIRST.  This one allocates the elem[] array. */
		delete [] elem; nelems=0;
		if (p>0) { nelems=p; elem=new db_mlselement[nelems]; }
		return true;
	}

	bool db_mlinestyle::set_6(char **p, int n, db_drawing *dp) {
		int fi1;
		if (p!=NULL && dp!=NULL) {
			for (fi1=0; fi1<nelems; fi1++) {
				elem[fi1].lthip=(fi1<n && p[fi1]!=NULL) ?
					dp->findtabrec(DB_LTYPETAB,p[fi1],1) : NULL;

				//if no linetype was found, set it to continuous.  This is done as a safeguard, because linteypes
				//in Mlinestyles are not being copied correctly.
				if (elem[fi1].lthip == NULL)
					elem[fi1].lthip = dp->findtabrec(DB_LTYPETAB,db_str_continuous,1);
			}
		}
		return true;
	}

	bool set_49(sds_real *p, int n) {
		int fi1;
		if (p!=NULL)
			for (fi1=0; fi1<nelems; fi1++)
				elem[fi1].offset=(fi1<n) ? p[fi1] : 0.0;
		return true;
	}
	bool set_62(int *p, int n) {
		int fi1;
		if (p!=NULL)
			for (fi1=0; fi1<nelems; fi1++)
				elem[fi1].color=(fi1<n) ? p[fi1] : DB_BLCOLOR;
		return true;
	}

  private:

	char		   deleted;

	char		  *name;	   /*	2 */
	char		  *desc;	   /*	3 */
	sds_real	   ang[2];	   /*  51,52 */
	int 		   fillcolor;  /*  62  CAREFUL!  Each element has a 62, also. */
	int 		   flags;	   /*  70 */
	int 		   nelems;	   /*  71 */
	db_mlselement *elem;	   /* Array of elements (nelements of them). */
};


class db_object_ptr : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_object_ptr(void);
	db_object_ptr(const db_object_ptr &sour);  /* Copy constructor */
   ~db_object_ptr(void);  /* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	void  setResbufChain(struct sds_resbuf* p) { m_pResbufChain = p; }
	struct sds_resbuf* getResbufChain() { return m_pResbufChain; }
	void  setOdDbClass(void* p) { m_OdClass = p; }
	void* getOdDbClass() { return m_OdClass; }

	// Note: db_object_ptr is not necessary entgespecific and entmod.
  private:

	char  deleted;

	// Holds the data for an OdDb-data.
	struct sds_resbuf* m_pResbufChain;
	void* m_OdClass;
};


class DB_CLASS db_rastervariables : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_rastervariables(void);
	db_rastervariables(const db_rastervariables &sour);  /* Copy constructor */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	// Note: db_rastervariables is not necessary entgespecific and entmod.
	bool get_70(int *p) { *p=dxf70; return true; }
	bool get_71(int *p) { *p=dxf71; return true; }
	bool get_72(int *p) { *p=dxf72; return true; }
	bool get_90(int *p) { *p=dxf90; return true; }

	bool set_70(int p) { dxf70=p; return true; }
	bool set_71(int p) { dxf71=p; return true; }
	bool set_72(int p) { dxf72=p; return true; }
	bool set_90(int p) { dxf90=p; return true; }

  private:

	char  deleted;

	/* Totally based on MarComp: */

	int dxf70;
	int dxf71;
	int dxf72;
	int dxf90;
};

class DB_CLASS db_spatial_filter : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_spatial_filter(void);
	db_spatial_filter(const db_spatial_filter &sour);  /* Copy constructor */
   ~db_spatial_filter(void);  /* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	int ret_npts(void) { return npts; }
	void get_ptarray(sds_point *p, int n) {
		if (n>npts) n=npts;
		//if (p!=NULL && pt!=NULL && n>0) memcpy(p,pt,n*sizeof(p[0]));
		if (p!=NULL && pt!=NULL && n>0) memcpy(p,pt,n*sizeof(sds_point));
	}
	void get_extru(sds_point p) {
		if (extru==NULL) { p[0]=p[1]=0.0; p[2]=1.0; } else DB_PTCPY(p,extru);
	}
	int 	 ret_frontclipon(void) { return clipon[0]; }
	int 	 ret_backclipon(void)  { return clipon[1]; }
	sds_real ret_frontclip(void)   { return clip[0];   }
	sds_real ret_backclip(void)    { return clip[1];   }

	bool get_11(sds_point p) { DB_PTCPY(p,dxf11); return true; }
	bool get_71(int *p) { *p=dxf71; return true; }

	void get_inverseblockmat(sds_point *p) { memcpy(p,inverseblockmat,sizeof(inverseblockmat)); }
	void get_clipboundmat(sds_point *p)    { memcpy(p,clipboundmat	 ,sizeof(clipboundmat)); }

	void set_ptarray(sds_point *p, int n) {  /* Sets npts, too */
		delete [] pt;
		pt = NULL;
		npts = 0;
		if(p && n > 0)
		{
			npts = n;
			if(!(pt = new sds_point[npts]))
				return;
			//memcpy(pt, p, npts * sizeof(pt[0]));
			memcpy(pt, p, npts * sizeof(sds_point));
			for(int i = 0; i < npts; i++)	/*D.G.*/// Don't set
				pt[i][2] = 0.0;				// non-zero values.
		}
	}
	void set_extru(sds_point p)    { db_aextrucpy(&extru,p); }
	void set_frontclipon(int p)    { clipon[0]=p; }
	void set_backclipon(int p)	   { clipon[1]=p; }
	void set_frontclip(sds_real p) { clip[0]=p; }
	void set_backclip(sds_real p)  { clip[1]=p; }

	bool set_11(sds_point p) { DB_PTCPY(dxf11,p); return true; }
	bool set_71(int p) { dxf71=p; return true; }

	void set_inverseblockmat(sds_point *p) { memcpy(inverseblockmat,p,sizeof(inverseblockmat)); }
	void set_clipboundmat(sds_point *p)    { memcpy(clipboundmat   ,p,sizeof(clipboundmat)); }

	// EBATECH(CNBR) -[ 2002/10/10 Bugzilla78218
	// Note: db_spatial_filter has entgetspecific and entmod.
	int entgetspecific(struct resbuf **begpp,struct resbuf **endpp,db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp);
	// EBATECH(CNBR) ]-

  private:

	char  deleted;

	/* Totally based on MarComp: */

	int 	   npts;		/*	2 */
	sds_point *pt;			/* 10(multiple) */
	sds_real  *extru;		/* 210 NULL means (0,0,1), just like in entities. */
	int 	   clipon[2];	/* 72 0:front; 73 1:back */
	sds_real   clip[2];		/* 40 0:front; 41 1:back */
	sds_point  dxf11;	   /*  11 origin of local coordinate system */
	int 	   dxf71;	   /*  71 clipping frame display 0:Off 1:On*/
	sds_point inverseblockmat[4];  /* 40 / 4x3 matrix */
	sds_point clipboundmat[4];	   /* 40 / 4x3 matrix */
};


class db_spatial_index : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_spatial_index(void);
	db_spatial_index(const db_spatial_index &sour);  /* Copy constructor */
   ~db_spatial_index(void);  /* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	int  ret_databits(void)  { return databits; }
	void set_databits(int p) { databits=p; }

	int  ret_timeval0(void)  { return timeval[0]; }
	int  ret_timeval1(void)  { return timeval[1]; }
	void set_timeval0(int p) { timeval[0]=p; }
	void set_timeval1(int p) { timeval[1]=p; }

	char *ret_data(void)   { return data; }
	int   ret_datasz(void) { return datasz; }

	void set_data(char *pdata, int pdatasz) {  /* Also sets datasz. */
		delete [] data; data=NULL; datasz=0;
		if (pdata!=NULL && pdatasz>0) {
			datasz=pdatasz; data=new char[datasz]; memcpy(data,pdata,datasz);
		}
	}
	// EBATECH(CNBR) -[ 2002/10/10 Bugzilla78218
	// Note: db_spatial_index has entgetspecific only
	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp) { return 0; }

	char *ret_data310(void)   { return data310; }
	int   ret_data310sz(void) { return data310sz; }

	void set_data310(char *pdata, int pdatasz) {  /* Also sets datasz. */
		delete [] data310; data310=NULL; data310sz=0;
		if (pdata!=NULL && pdatasz>0) {
			data310sz=pdatasz; data310=new char[data310sz]; memcpy(data310,pdata,data310sz);
		}
	}
	bool get_40(sds_real *p, int idx) { if( p != NULL && idx > -1 && idx < 6 ){ *p = dxf40[idx]; } return true; }
	bool get_40array(sds_real *p, int n) {	if( p ){ memcpy( p, dxf40, sizeof(sds_real) * min( n, 6 )); } return true; }
	bool set_40(sds_real p, int idx) { if( idx > -1 && idx < 6 ){ dxf40[idx] = p; } return true; }
	bool set_40(sds_real *p, int n) { if( p ){ memcpy(dxf40, p, sizeof(sds_real) * min( n, 6 )); } return true; }
	db_handitem *ret_parentinsert(void) { return parentinsert; }
	db_handitem **retp_parentinsert(void) { return &parentinsert; } // for fixups
	// EBATECH(CNBR) ]-

	void  setResbufChain(struct sds_resbuf* p) { m_pResbufChain = p; }
	struct sds_resbuf* getResbufChain() { return m_pResbufChain; }
	void  setOdDbClass(void* p) { m_OdClass = p; }
	void* getOdDbClass() { return m_OdClass; }

  private:

	char  deleted;

	/* Totally based on MarComp: */

	int   databits;
	int   timeval[2];	// 40 : Timestamp
	int   datasz;
	char *data;
	// EBATECH(CNBR) -[ 2002/10/10 Bugzilla78218
	int   data310sz;
	char *data310;		// 310 : unknown
	sds_real dxf40[6];	// 40 : unknown
	db_handitem* parentinsert; // 330
	// EBATECH(CNBR) ]-

	// Holds the data for an OdDb-data.
	struct sds_resbuf* m_pResbufChain;
	void* m_OdClass;
};


class DB_CLASS db_xrecord : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_xrecord(void);
	db_xrecord(const db_xrecord &sour);  /* Copy constructor */
   ~db_xrecord(void);  /* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	int entgetspecific(struct resbuf **begpp,struct resbuf **endpp,db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp);


	void setResbufChain(struct sds_resbuf* rb) { m_pResbufChain = rb; }
	const struct sds_resbuf* getResbufChain() { return m_pResbufChain; }
  private:

	char  deleted;

	struct resbuf* m_pResbufChain;
};

class DB_CLASS db_placeholder : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_placeholder(void);
	db_placeholder(const db_placeholder &sour);  /* Copy constructor */
   ~db_placeholder(void);  /* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	//short ret_unused(void) { return unused; }
	//void	set_unused(short s) { unused=s; }
	db_handitem **retp_ownerItem(void) { return &ownerItem; } // for fixups
	db_handitem *ret_ownerItem(void) { return ownerItem; } // for fixups
	void set_ownerItem(db_handitem *p) { ownerItem = p; }

  private:

	char  deleted;

	/* Totally based on MarComp:
		2002/9/5 ebatech(cnbr)
		Unused member exists only for compounding union. so removed.
	*/
	//short unused;
	db_handitem* ownerItem;	// 2002/9/5 for fixup
};

class DB_CLASS db_wipeoutvariables : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_wipeoutvariables(void);
	db_wipeoutvariables(const db_wipeoutvariables &sour);  /* Copy constructor */
   ~db_wipeoutvariables(void);	/* Implicitly virtual */

	// EBATECH(CNBR) -[ 2002/10/10 Bugzilla#78235
	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	//short   ret_dxf70(void)  { return dxf70; }
	//void	set_dxf70(short d) { dxf70=d; }
	bool get_70(int *p) { *p=dxf70; return true; }
	bool set_70(int p) { dxf70=p; return true; }
	// EBATECH(CNBR) ]-

  private:
	char		  deleted;

	/* Totally based on MarComp: */
	short dxf70;
};

class DB_CLASS db_vbaproject : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_vbaproject(void);
	db_vbaproject(const db_vbaproject &sour);  /* Copy constructor */
   ~db_vbaproject(void);  /* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	long  ret_datasize(void)  { return datasize; }
	char *ret_data(void)  { return data; }
	void  set_data(long ds,char *dt)
		{
		datasize=ds;
		delete [] data;
		data = new char[datasize];
		memcpy(data,dt,datasize);
		}

	// Note: db_vbaproject cannot update its data now.
	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp) { return 0; }

  private:
	char		  deleted;

	/* Totally based on MarComp: */
	long datasize;
	char *data;
};

class DB_CLASS db_dictionarywdflt : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

	db_dictionarywdflt(void);
	db_dictionarywdflt(const db_dictionarywdflt &sour);  /* Copy constructor */
   ~db_dictionarywdflt(void);  /* Implicitly virtual */

	int   ret_deleted(void)  { return deleted; }
	void  set_deleted(int p) { deleted=(char)(p!=0); }

	long ret_numitems(void) { return numitems; }
	void  set_numitems(long s) { numitems=s; }
	// EBATECH(CNBR) -[ 2002/6/30 Bug#78227
	//short ret_cloningflag(void) { return cloningflag; }
	//void	set_cloningflag(/*unsigned*/ short s) { cloningflag=s; }	/*D.G.*/// Commented out the 'unsigned' word.
	// EBATECH(CNBR) ]-

	void delrecll(void) {
		while (recll[0]!=NULL)
			{ recll[1]=recll[0]->next; delete recll[0]; recll[0]=recll[1]; }
		recll[0]=recll[1]=acadnextrec=icadnextrec=NULL;
		numitems = 0; // EBATECH(CNBR) 2002/6/30 Bug#78227
	}
	db_handitem **retp_defaultobject(void) { return &defaultobject; } // for fixups
	db_handitem *ret_defaultobject(void) { return defaultobject; } // for fixups
	void get_recllends(db_dictionaryrec **firstpp, db_dictionaryrec **lastpp) {
		if (firstpp!=NULL) *firstpp=recll[0];
		if (lastpp !=NULL) *lastpp =recll[1];
	}
	void set_recllends(db_dictionaryrec *firstp, db_dictionaryrec *lastp) {
		recll[0]=firstp; recll[1]=lastp;
	}
	void addrec(db_dictionaryrec *toadd) {
		if (toadd!=NULL) {
			if (recll[0]==NULL) recll[0]=acadnextrec=icadnextrec=toadd;
			else recll[1]->next=toadd;
			recll[1]=toadd;
			++numitems; // EBATECH(CNBR) 2002/6/30 Bug#78227
		}
	}
	int delrec(char *recname) {
	/*
	**	Returns:
	**		 0 : OK
	**		-1 : recname is NULL or ""
	**		-2 : recname not found
	*/
		db_dictionaryrec *tp1,*tp2;

		if (recname==NULL || !*recname) return -1;
		for (tp1=NULL,tp2=recll[0]; tp2!=NULL && (tp2->name==NULL ||
			db_compareNames(tp2->name,recname)); tp1=tp2,tp2=tp2->next);
		if (tp2==NULL) return -2;
		if (tp1==NULL) recll[0]=tp2->next;
		else		  tp1->next=tp2->next;
		if (tp2->next==NULL) recll[1]=tp1;
		if (acadnextrec==tp2) acadnextrec=tp2->next;
		if (icadnextrec==tp2) icadnextrec=tp2->next;
		delete tp2;
		--numitems; // EBATECH(CNBR) 2002/6/30 Bug#78227
		return 0;
	}
	db_dictionaryrec *findrec(const char *name, int setnext, db_drawing *dp) {
		db_dictionaryrec *found=NULL;

		if (name!=NULL && *name) {
			for (found=recll[0]; found!=NULL &&
				(found->name==NULL || db_compareNames(found->name,name)); found=found->next);
		}
		if (setnext && found!=NULL) {
			acadnextrec=icadnextrec=found;
			stepacadnextrec(dp); stepicadnextrec();
		}
		return found;
	}
	// Ebatech(cnbr) -[ 2002/9/2 Bugzilla78227
	int renamerec(char *oldname, char * newname, db_drawing *dp) {
		db_dictionaryrec *thisrec;

		if (oldname==NULL || !*oldname ||
			newname==NULL || !*newname) 	 return -1;  /* Param NULL or "" */
		if ((thisrec=findrec(oldname,0,dp))==NULL) return -2;  /* Can't find old */
		if (findrec(newname,0,dp)!=NULL)		   return -3;  /* New already exists. */
		db_astrncpy(&thisrec->name,newname,-1);
		if (thisrec->name!=NULL) _tcsupr(thisrec->name);
		return 0;
	}
	db_dictionaryrec *ret_acadnextrec(void) { return acadnextrec; }
	db_dictionaryrec *ret_icadnextrec(void) { return icadnextrec; }
	// ebatech(cnbr) ]-
	void set_acadnextrec(db_dictionaryrec *p) { acadnextrec=p; }
	void set_icadnextrec(db_dictionaryrec *p) { icadnextrec=p; }

	int ret_nrecs(void) {
		int rc;
		db_dictionaryrec *tp;

		for (tp=recll[0],rc=0; tp!=NULL; tp=tp->next,rc++);
		return rc;
	}

	void stepacadnextrec(db_drawing *dp) {	/* ACAD-style.	See comments above. */
		db_handitem *thip1;

		if (acadnextrec==NULL || acadnextrec->hiref==NULL ||
			acadnextrec->hiref->ret_hip(dp)==NULL) {

			acadnextrec=NULL;
		} else {
			thip1=acadnextrec->hiref->ret_hip(dp);
			for (acadnextrec=recll[0]; acadnextrec!=NULL &&
				(acadnextrec->hiref==NULL ||
				acadnextrec->hiref->ret_hip(dp)==NULL ||
				acadnextrec->hiref->ret_hip(dp)!=thip1); acadnextrec=acadnextrec->next);
			acadnextrec=(acadnextrec==NULL) ? NULL : acadnextrec->next;
		}
	}
	void stepicadnextrec(void) {  /* OURS.	See comments above. */
		icadnextrec=(icadnextrec==NULL) ? NULL : icadnextrec->next;
	}

	// EBATECH(CNBR) -[ 2002/6/30 Bug#78227
	bool get_281(int *p) { *p=cloningflag; return true; }
	bool get_340(db_handitem **p) { *p=defaultobject; return true; }

	bool set_281(int p) { cloningflag = (( p < 0 || p > 5 ) ? 1 : p); return true; }
	bool set_340(db_handitem *p) { defaultobject=p; return true; }

	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp);
	// EBATECH(CNBR) ]-

  private:

	char  deleted;

	/* Totally based on MarComp: */
//	unsigned short numitems;
	long numitems;	// new version of ODWG requires long
	short cloningflag; /* 281 */
	db_handitem *defaultobject; /* 340 */
	db_dictionaryrec *recll[2];  /* llist of records */
	db_dictionaryrec *acadnextrec;	/* Ptr into recll for ACAD-style "dictnext". */
	db_dictionaryrec *icadnextrec;	/* Ptr into recll for OUR "dictnext". */
};

// EBATECH(CNBR) -[ 2002/6/30 Bug#78227
class db_dimassocelement {
	public:
	db_dimassocelement(void) {
		name=NULL;
		osSnapType = mainSubEntType = hasLastPointRef = 0;
		mainGsMarker = subEntIntType = intGsMarker = 0;
		nearOsnap = osnapPoint[0] = osnapPoint[1] = osnapPoint[2] = 0.0;
		mainObjHandle = intObjHandle = NULL;
		// Ebatech(cnbr) -[ xrefObjHandle cannot translate Handle to Entity name
		xrefHandleCount = 0L;
		//xrefObjHandle = intXrefObjHandle = NULL;
		// Ebatech(cnbr) ]-
	}
	~db_dimassocelement(void) {
		if( name ) delete [] name;
	}

	char *name;					// 1 used as flag. name!=NULL means filled properties.
	short osSnapType;			// 72
	short mainSubEntType;		// 73
	int mainGsMarker;			// 91
	double nearOsnap;			// 40
	double osnapPoint[3];		// 10
	short hasLastPointRef;		// 75
	db_handitem* mainObjHandle; // 331
	db_handitem* intObjHandle;	// 332
	int subEntIntType;			// 74
	int intGsMarker;			// 92
	// Ebatech(cnbr) -[ xrefObjHandle cannot translate Handle to Entity name
	long xrefHandleCount;
	db_objhandle xrefObjHandles[8];	// 301
	db_objhandle intXrefObjHandle;	// 302
	//db_handitem* xrefObjHandle; 	// 301
	//db_handitem* intXrefObjHandle;// 302
	// Ebatech(cnbr) ]-
};

class DB_CLASS db_dimassoc : public db_handitem {

	friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

	public:

	db_dimassoc(void);
	db_dimassoc(const db_dimassoc &sour);  /* Copy constructor */
   ~db_dimassoc(void);	/* Implicitly virtual */

	int  ret_deleted(void)	{ return deleted; }
	void set_deleted(int p) { deleted=(char)(p!=0); }

	// Note: db_dimassoc has entgetspecific and entmod.
	//		 Now entmod cannot modify any properties.
	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing *dp);
	int entmod(struct resbuf *modll, db_drawing *dp) { return 0; }

	// These functions used for fixup object IDs.
	db_handitem **retp_dimensionObjHandle(void)  { return &dimensionObjHandle; }
	db_handitem **retp_mainObjHandle(int idx)	 { return &elements[idx].mainObjHandle; }
	db_handitem **retp_intObjHandle(int idx)	 { return &elements[idx].intObjHandle; }
	// Ebatech(cnbr) -[ xrefObjHandle cannot translate Handle to Entity name
	//db_handitem **retp_xrefObjHandle(int idx)    { return &elements[idx].xrefObjHandle; }
	//db_handitem **retp_intXrefObjHandle(int idx) { return &elements[idx].intXrefObjHandle; }
	// Ebatech(cnbr) ]-

	bool get_330(db_handitem **p) { *p = dimensionObjHandle; return true; }
	bool get_90(int *p) { *p = associativityFlag; return true; }
	bool get_70(int *p) { *p = transSpaceFlag; return true; }
	bool get_71(int *p) { *p = rotatedDimType; return true; }
	// These functions require an index argument.
	bool get_1(char **p, int idx) { *p = elements[idx].name; return true; }
	bool get_1(char *p, int n, int idx) { strncpy(p, elements[idx].name, n); return true; }
	bool get_72(int *p, int idx) { *p = elements[idx].osSnapType; return true; }
	bool get_73(int *p, int idx) { *p = elements[idx].mainSubEntType; return true; }
	bool get_91(int *p, int idx) { *p = elements[idx].mainGsMarker; return true; }
	bool get_40(sds_real *p, int idx) { *p = elements[idx].nearOsnap; return true; }
	bool get_10(sds_point p, int idx) { DB_PTCPY( p, elements[idx].osnapPoint); return true; }
	bool get_75(int *p, int idx) { *p = elements[idx].hasLastPointRef; return true; }
	bool get_331(db_handitem **p, int idx) { *p = elements[idx].mainObjHandle; return true; }
	bool get_332(db_handitem **p, int idx) { *p = elements[idx].intObjHandle; return true; }
	bool get_74(int *p, int idx) { *p = elements[idx].subEntIntType; return true; }
	bool get_92(int *p, int idx) { *p = elements[idx].intGsMarker; return true; }
	// Ebatech(cnbr) -[ xrefObjHandle cannot translate Handle to Entity name
	//bool get_301(db_handitem **p, int idx) { *p = elements[idx].xrefObjHandle; return true; }
	//bool get_302(db_handitem **p, int idx) { *p = elements[idx].intXrefObjHandle; return true; }
	long ret_xrefHandleCount(int idx ) { return elements[idx].xrefHandleCount; }
	bool get_xrefObjHandle(int idx,  unsigned char odhandle[8], int num = 0)
		{ elements[idx].xrefObjHandles[num].ToODHandle(odhandle); return true; }
	bool get_intXrefObjHandle(int idx, unsigned char odhandle[8])
		{ elements[idx].intXrefObjHandle.ToODHandle(odhandle); return true;}
	// Ebatech(cnbr) ]-

	bool set_330(db_handitem *p) { dimensionObjHandle = p; return true; }
	bool set_90(int p) { associativityFlag = ( p & 0x0f ); return true; }
	bool set_70(int p) { transSpaceFlag = ( p == 0 ? 0 : 1); return true; }
	bool set_71(int p) { rotatedDimType = p; return true; }
	// These functions require an index argument.
	bool set_1(char *p, int idx)
	{
		db_astrncpy(&elements[idx].name,(p==NULL) ? db_str_quotequote : p,-1);
		return true;
	}
	bool set_72(int p, int idx) { elements[idx].osSnapType = p; return true; }
	bool set_73(int p, int idx) { elements[idx].mainSubEntType = p; return true; }
	bool set_91(int p, int idx) { elements[idx].mainGsMarker = p; return true; }
	bool set_40(sds_real p, int idx) { elements[idx].nearOsnap = p; return true; }
	bool set_10(sds_point p, int idx) { DB_PTCPY( elements[idx].osnapPoint, p); return true; }
	bool set_75(int p, int idx) { elements[idx].hasLastPointRef = p; return true; }
	bool set_331(db_handitem *p, int idx) { elements[idx].mainObjHandle = p; return true; }
	bool set_332(db_handitem *p, int idx) { elements[idx].intObjHandle = p; return true; }
	bool set_74(int p, int idx) { elements[idx].subEntIntType = p; return true; }
	bool set_92(int p, int idx) { elements[idx].intGsMarker = p; return true; }
	// Ebatech(cnbr) -[ xrefObjHandle cannot translate Handle to Entity name
	//bool set_301(db_handitem *p, int idx) { elements[idx].xrefObjHandle = p; return true; }
	//bool set_302(db_handitem *p, int idx) { elements[idx].intXrefObjHandle = p; return true; }
	bool set_xrefHandleCount(int idx, long p )
		{ elements[idx].xrefHandleCount=p; return true; }
	bool set_xrefObjHandle(int idx,  unsigned char odhandle[8], int num = 0)
		{ elements[idx].xrefObjHandles[num] = db_objhandle(odhandle); return true; }
	bool set_intXrefObjHandle(int idx, unsigned char odhandle[8])
		{ elements[idx].intXrefObjHandle = db_objhandle(odhandle); return true;}
	// Ebatech(cnbr) ]-

	private:

	char  deleted;

	/* Totally based on MarComp: */
	db_handitem *dimensionObjHandle;	// 330
	int associativityFlag;				// 90
	short transSpaceFlag;				// 70
	short rotatedDimType;				// 71
	db_dimassocelement elements[8];
};
// EBATECH(CNBR) ]-
#endif
