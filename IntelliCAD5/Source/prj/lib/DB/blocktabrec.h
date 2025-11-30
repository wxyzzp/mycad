/* C:\ICAD\PRJ\LIB\DB\BLOCKTABREC.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#ifndef _INC_BLOCKTABREC
#define _INC_BLOCKTABREC

class DB_CLASS db_blocktabrec : public db_tablerecord {

    friend int db_CompEntLinks(struct db_BinChange **chglst, db_handitem *oldhip, db_handitem *newhip);

  public:

    db_blocktabrec(void);
    db_blocktabrec(char *pname, db_drawing *dp);
    db_blocktabrec(const db_blocktabrec &sour);  /* Copy constructor */
	~db_blocktabrec(void);

	virtual int check(db_drawing* pDrawing);
	void db_blocktabrec::addent(db_handitem *p, db_drawing *dp);
	void db_blocktabrec::delent(db_handitem *p, db_drawing *dp);

    void steal_blockredef(db_handitem *first, db_handitem *last) {
        db_handitem::delll(oldentll[0]);
        oldentll[0]=entll[0]; oldentll[1]=entll[1];
        entll[0]=entll[1]=NULL;
        if (first!=NULL && last!=NULL) //{ entll[0]=first; entll[1]=last; }
			set_blockllends( first, last );
    }

    void get_blockllends(db_handitem **firstpp, db_handitem **lastpp) {
        if (firstpp!=NULL) *firstpp=entll[0];
        if (lastpp !=NULL) *lastpp =entll[1];
    }
    void set_blockllends(db_handitem *firstp, db_handitem *lastp) {
        entll[0]=firstp; entll[1]=lastp;

		// set m_parentBlock to this pointer to all objects in ll
		for( db_handitem *hip = firstp; hip; hip = hip->next )
			hip->m_parentBlock = this;
    }

    void get_oldblockllends(db_handitem **firstpp, db_handitem **lastpp) {
        if (firstpp!=NULL) *firstpp=oldentll[0];
        if (lastpp !=NULL) *lastpp =oldentll[1];
    }
    void set_oldblockllends(db_handitem *firstp, db_handitem *lastp) {
        oldentll[0]=firstp; oldentll[1]=lastp;

		// set m_parentBlock to this pointer to all objects in ll
		for( db_handitem *hip = firstp; hip; hip = hip->next )
			hip->m_parentBlock = this;
    }

	void set_description(char *desc)
		{
		strncpy(description,desc,sizeof(description));
		}

	void get_description(char *desc)
		{
		strncpy(desc,description,sizeof(description));
		}

	bool get_firstentinblock(sds_name p, db_drawing *dp);
    bool get_firstentinblock(db_handitem **p);


	db_block *ret_block( void )
		{
		ASSERT( entll[0] != NULL );
		ASSERT( entll[0]->ret_type() == DB_BLOCK );

		return (db_block *)entll[0];
		}

    int ret_looked4xref(void) {
        return (entll[0]!=NULL) ? entll[0]->ret_looked4xref() : 0;
    }
    db_drawing *ret_xrefdp(void) {
        return (entll[0]!=NULL) ? entll[0]->ret_xrefdp() : NULL;
    }
    bool get_1(char *p, int maxlen) {
        if (entll[0]!=NULL) entll[0]->get_1(p,maxlen); else *p=0;
        return true;
    }
    bool get_1(char **p) {
        if (entll[0]!=NULL) entll[0]->get_1(p); else *p=NULL;
        return true;
    }
    
	bool get_3(char *p, int maxlen) {
        if (entll[0]!=NULL) entll[0]->get_3(p,maxlen); else *p=0;
        return true;
    }
    bool get_3(char **p) {
        if (entll[0]!=NULL) entll[0]->get_3(p); else *p=NULL;
        return true;
    }
    bool get_10(sds_point p) {
        if (entll[0]!=NULL) entll[0]->get_10(p);
        else { p[0]=p[1]=p[2]=0.0; }
        return true;
    }

	void set_looked4xref(int p) { if (entll[0]!=NULL) entll[0]->set_looked4xref(p); }
    void set_xrefdp(db_drawing *p) { if (entll[0]!=NULL) entll[0]->set_xrefdp(p); }
    void null_xrefdp(void) { if (entll[0]!=NULL) entll[0]->null_xrefdp(); }
    bool set_1(char *p) {
        if (entll[0]!=NULL) entll[0]->set_1(p);
        return true;
    }
    bool set_3(char *p) {
        if (entll[0]!=NULL) entll[0]->set_3(p);
        return true;
    }
    bool set_10(sds_point p) {
        if (entll[0]!=NULL) entll[0]->set_10(p);
        return true;
    }

	bool db_blocktabrec::set_2(char*);
	bool db_blocktabrec::set_70(int p);

	void set_layoutobjhandle(db_objhandle lh)
		{
		layoutobjhandle=lh;
		}

	void get_layoutobjhandle(db_objhandle *lh)
		{
		*lh=layoutobjhandle;
		}

	bool db_blocktabrec::set_2(char *name, db_drawing *dp) {
/*
**  If it's an anonymous block; add the number.
**
**  This one calls it's own set_2(char *), which will set the db_block
**  "name", too.
*/
	    char *astr=NULL;

		// check to see if name generation is on for wblock
		if (dp->GeneratingAnonNames())
			{
			astr=db_makeanonname(name,dp, 1);	/*D.G.*/// Why we should generate a new name,
												// if we have already have it? So, pass 1, not 0.
			set_2(astr);
			delete [] astr;
			}
		else
			set_2(name);

	    return true;
	}

	int entgetspecific(struct resbuf **begpp, struct resbuf **endpp, db_drawing     *dp); 

  private:
    /*
    **  db_blocktabrec and db_block have duplicate name and flags data.
    **  See note in db_block.  For all other data (except entll), the
    **  db_blocktabrec getters and setters access the db_block's data
    **  (via entll[0]).  entll has to be in db_blocktabrec (because
    **  it points to the db_block via entll[0], for one thing).
    */
    db_handitem  *entll[2]; /* [0] : llist of ents; [1] : last link (chaining).
                            /*   Begins with the BLOCK entity.  (The -2 */
                            /*   is the 1st ent in the def (after the BLOCK)) */
    db_handitem  *oldentll[2];  /* Temporarily holds the old block def */
                                /* when the block is redefined (for undo */
                                /* purposes). */
#ifdef ODTVER_2_009			// Ebatech(CNBR) 2002/5/30 Expand size
	char description[1024];
#else
	char description[512];
#endif
	db_objhandle layoutobjhandle;
};

#endif

